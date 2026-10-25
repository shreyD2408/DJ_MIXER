
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <I2S.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED
#define OLED_SDA 0
#define OLED_SCL 1
#define OLED_WIDTH 128
#define OLED_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(
  OLED_WIDTH, OLED_HEIGHT, &Wire, OLED_RESET
);

// CD74HC4067 multiplexer
#define MUX_SIG 26
#define MUX_S0 2
#define MUX_S1 3
#define MUX_S2 4
#define MUX_S3 5

// Buttons
const int buttons[] = {6, 7, 8, 9};
const char *sampleFiles[] = {
  "/SAMPLE1.WAV",
  "/SAMPLE2.WAV",
  "/SAMPLE3.WAV",
  "/SAMPLE4.WAV"
};

// MicroSD: SPI0
#define SD_CS 17
#define SD_SCK 18
#define SD_MOSI 19
#define SD_MISO 16

// MAX98357A
// On Arduino-Pico, LRCLK is BCLK + 1.
#define I2S_BCLK 10
#define I2S_DATA 12

I2S audio(OUTPUT, I2S_BCLK, I2S_DATA);

int potValues[6] = {0, 0, 0, 0, 0, 0};
bool lastButtonState[4] = {HIGH, HIGH, HIGH, HIGH};
unsigned long lastButtonTime[4] = {0, 0, 0, 0};

bool sdReady = false;
bool oledReady = false;
bool audioReady = false;
bool isPlaying = false;

const unsigned long debounceTime = 30;

// Select a multiplexer channel and read its voltage
int readMux(int channel) {
  digitalWrite(MUX_S0, channel & 1);
  digitalWrite(MUX_S1, (channel >> 1) & 1);
  digitalWrite(MUX_S2, (channel >> 2) & 1);
  digitalWrite(MUX_S3, (channel >> 3) & 1);

  delayMicroseconds(100);

  // Discard the first reading after changing channels
  analogRead(MUX_SIG);

  return analogRead(MUX_SIG);
}

// Read all six knobs
void readPotentiometers() {
  for (int i = 0; i < 6; i++) {
    potValues[i] = readMux(i);
  }
}

// Show status and knob positions
void updateDisplay(const char *status) {
  if (!oledReady) return;

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);

  display.setCursor(0, 0);
  display.println("PICO DJ CONTROLLER");

  display.setCursor(0, 12);
  display.print("Status: ");
  display.println(status);

  for (int i = 0; i < 6; i++) {
    display.setCursor((i % 2) * 64, 26 + (i / 2) * 12);
    display.print("K");
    display.print(i + 1);
    display.print(":");
    display.print(map(potValues[i], 0, 4095, 0, 100));
    display.print("%");
  }

  display.display();
}

// Play a 16-bit PCM mono WAV file
// Expected format: standard 44-byte WAV header,
// 22050 Hz, uncompressed PCM, 16-bit mono.
void playSample(const char *filename) {
  if (!sdReady || !audioReady) {
    Serial.println("SD or audio is not ready.");
    updateDisplay("Hardware error");
    return;
  }

  File file = SD.open(filename, FILE_READ);

  if (!file) {
    Serial.print("Could not open: ");
    Serial.println(filename);
    updateDisplay("File missing");
    return;
  }

  // Read and validate the standard WAV header
  uint8_t header[44];

  if (file.read(header, sizeof(header)) != sizeof(header)) {
    Serial.println("Invalid WAV header.");
    file.close();
    updateDisplay("Bad WAV");
    return;
  }

  if (memcmp(header, "RIFF", 4) != 0 ||
      memcmp(header + 8, "WAVE", 4) != 0 ||
      memcmp(header + 12, "fmt ", 4) != 0 ||
      memcmp(header + 36, "data", 4) != 0) {
    Serial.println("Unsupported WAV layout.");
    file.close();
    updateDisplay("Bad WAV");
    return;
  }

  uint16_t format =
    header[20] | (header[21] << 8);

  uint16_t channels =
    header[22] | (header[23] << 8);

  uint32_t sampleRate =
    (uint32_t)header[24] |
    ((uint32_t)header[25] << 8) |
    ((uint32_t)header[26] << 16) |
    ((uint32_t)header[27] << 24);

  uint16_t bitsPerSample =
    header[34] | (header[35] << 8);

  uint32_t dataSize =
    (uint32_t)header[40] |
    ((uint32_t)header[41] << 8) |
    ((uint32_t)header[42] << 16) |
    ((uint32_t)header[43] << 24);

  if (format != 1 || channels != 1 ||
      bitsPerSample != 16 || sampleRate != 22050) {
    Serial.println(
      "Use PCM, mono, 16-bit, 22050 Hz WAV files."
    );
    file.close();
    updateDisplay("Wrong WAV format");
    return;
  }

  audio.end();

  if (!audio.begin(sampleRate)) {
    Serial.println("I2S restart failed.");
    file.close();
    audioReady = false;
    updateDisplay("Audio error");
    return;
  }

  isPlaying = true;
  updateDisplay(filename);

  uint32_t bytesPlayed = 0;
  int16_t sample;

  while (bytesPlayed + 1 < dataSize &&
         file.available() >= 2) {

    if (file.read((uint8_t *)&sample, 2) != 2) {
      break;
    }

    bytesPlayed += 2;

    // Knob 1 controls playback volume
    int volume = map(potValues[0], 0, 4095, 0, 100);
    int32_t adjusted = (int32_t)sample * volume / 100;
    int16_t outputSample = (int16_t)adjusted;

    // I2S stereo output: same sound in both channels
    audio.write(outputSample);
    audio.write(outputSample);
  }

  file.close();
  isPlaying = false;

  updateDisplay("Ready");
  Serial.print("Finished: ");
  Serial.println(filename);
}

// Check for button presses
void checkButtons() {
  for (int i = 0; i < 4; i++) {
    bool currentState = digitalRead(buttons[i]);

    if (lastButtonState[i] == HIGH &&
        currentState == LOW &&
        millis() - lastButtonTime[i] >= debounceTime) {

      lastButtonTime[i] = millis();

      Serial.print("Button ");
      Serial.print(i + 1);
      Serial.println(" pressed");

      readPotentiometers();
      playSample(sampleFiles[i]);
    }

    lastButtonState[i] = currentState;
  }
}

void setup() {
  Serial.begin(115200);

  // Multiplexer
  pinMode(MUX_S0, OUTPUT);
  pinMode(MUX_S1, OUTPUT);
  pinMode(MUX_S2, OUTPUT);
  pinMode(MUX_S3, OUTPUT);

  analogReadResolution(12);

  // Buttons use internal pull-up resistors
  for (int i = 0; i < 4; i++) {
    pinMode(buttons[i], INPUT_PULLUP);
  }

  // OLED
  Wire.setSDA(OLED_SDA);
  Wire.setSCL(OLED_SCL);
  Wire.begin();

  oledReady = display.begin(
    SSD1306_SWITCHCAPVCC, 0x3C
  );

  if (oledReady) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("Starting DJ board...");
    display.display();
  } else {
    Serial.println("OLED not detected.");
  }

  // Configure SPI pins for the microSD module
  SPI.setRX(SD_MISO);
  SPI.setTX(SD_MOSI);
  SPI.setSCK(SD_SCK);
  SPI.setCS(SD_CS);

  sdReady = SD.begin(SD_CS);

  if (sdReady) {
    Serial.println("MicroSD ready.");
  } else {
    Serial.println("MicroSD initialization failed.");
  }

  // Initialize I2S audio output
  audio.setBitsPerSample(16);
  audioReady = audio.begin(22050);

  if (audioReady) {
    Serial.println("I2S audio ready.");
  } else {
    Serial.println("I2S initialization failed.");
  }

  readPotentiometers();
  updateDisplay("Ready");

  Serial.println("DJ controller initialized.");
  Serial.println("Press buttons 1-4 to play samples.");
}

void loop() {
  readPotentiometers();
  checkButtons();

  static unsigned long lastDisplayUpdate = 0;

  if (millis() - lastDisplayUpdate >= 150) {
    lastDisplayUpdate = millis();
    updateDisplay("Ready");
  }

  delay(5);
}
