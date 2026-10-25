# PICO W DJ Board

An inexpensive and easy-to-build DIY DJ controller and audio sampler using a Raspberry Pi Pico W. This project is a way of exploring Audio Electronics, Embedded Programming and Physical Music Controls by creating a device to play some sound samples by pressing the buttons and interact with the potentiometers in real time.

<img width="500" alt="image" src="https://github.com/user-attachments/assets/4f100043-d2f5-4838-b2c9-3bc889826ac8" />
<img width="500" alt="image" src="https://github.com/user-attachments/assets/fa8a1a5c-69b1-4b33-bdbe-14898a730ec5" />

Onshape Link: https://cad.onshape.com/documents/78497d0a712e723bfcc60a77/w/26bbe8603345eb683ec0ee8a/e/9154dd89120088dab20cdbc0?renderMode=0&uiState=6ade7a400bd5493da9bcfbe0

# Features

- Audio Sampling: Play WAV audio samples stored in a microSD card.
- Physical Controls: Four tactile buttons trigger indiviual samples.
- Potentiometer inputs: Six knobs provide analog inputs for future mixing and sound-control features.
- OLED display: Displays the current potentiometer readings and device status.
- Digital Audio Output: Uses a MAX98357A I2S amplifier to drive a speaker
- Modular hardware: Uses premade modules connected with jumper wires, making the project easier to assemble and modify.

# How it works

The raspberry Pi Pico W is the primary controller and is running the Arduino C++ firmware which collects inputs, accesses the microSD card, refreshed the OLED display and outputs the digital audio stream to the amplifier.

The Pico W can use a CD74HC4067 analog multiplexer to read multiple analog inputs via a single ADC pin. The 4 buttons play audio files on the microSD card, the SSD1306 OLED shows the current knob setting and playing state.

Pressing a button causes the Pico to read a WAV file from the microSD card and audio samples are sent to the MAX98357A amplifier over the I2S interface. The amplifier translates the digital signal from the computer into a signal to drive the speaker.

## Hardware Overview

| Component | Purpose |
|---|---|
| Raspberry Pi Pico W | Main controller for the entire device |
| SSD1306 OLED | Displays potentiometer readings and status |
| 6 × Potentiometers | Provide analog control inputs |
| CD74HC4067 Multiplexer | Connects six potentiometers to one ADC input |
| 4 × Tactile Buttons | Trigger audio samples |
| MicroSD Card Module | Stores audio files |
| MAX98357A Amplifier | Converts I²S digital audio into amplified speaker output |
| Speaker | Plays the audio samples |
| USB Power | Powers the Pico W during initial testing |

# Wiring Diagram

The project uses premade modules and jumper wires rather than a custom PCB. All modules share a common ground, and the Pico W's GPIO signals uses 3.3V logic.

<img width="317" height="373" alt="image" src="https://github.com/user-attachments/assets/d57d4d3a-b638-41b5-b2db-02c1e28a9480" />

### Raspberry Pi Pico W Pin Connections

| Module / Signal | Pico W Pin |
|---|---|
| OLED SDA | GP0 |
| OLED SCL | GP1 |
| Multiplexer S0 | GP2 |
| Multiplexer S1 | GP3 |
| Multiplexer S2 | GP4 |
| Multiplexer S3 | GP5 |
| Button 1 | GP6 |
| Button 2 | GP7 |
| Button 3 | GP8 |
| Button 4 | GP9 |
| MAX98357A BCLK | GP10 |
| MAX98357A LRC / WS | GP11 |
| MAX98357A DIN | GP12 |
| microSD MISO | GP16 |
| microSD CS | GP17 |
| microSD SCK | GP18 |
| microSD MOSI | GP19 |
| Multiplexer SIG / COM | GP26 (ADC0) |

Connect the multiplexer enable pin to GND, and connect its C0-C5 channels to the six potentiometer wipers. Each potentiometer's outer terminals connect to 3v3/5v and GND.

For each button, connect one side to its assigned GPIO pin and the other side to GND. The firmware uses internal pull-up resistors.

Connect the OLED, multiplexer, microSD module, and amplifier grounds to the Pico W's common ground. Connect the speaker only to the amplifier's SPK+ and SPK- terminals. Check the voltage requirements of your particular microSD and amplifier modules before powering them.

# Software

The firmware is written in Arduino C++ and is designed for the Raspberry Pi Pico W using the Arduino-Pico board core.

# Required Software and Libraries

- Arduino IDE
- Arduino-Pico board core
- Adafruit GFX Library
- Adafruit SSD1306 Library
- The SPI, SD, Wire, and I2S libraries provided by the selected board core.

# Setup

1. Install the Arduino IDE and the Arduino-Pico board core.
2. Select the Raspberry Pi Pico W board.
3. Install the Adafruit GFX and SSD1306 libraries through the Arduino Library Manager.
4. Connect the OLED, controls, microSD module and the MAX98357A amplifier according to the wiring table.
5. Format a compatible microSD card as **FAT32**.
6. Copy four supported WAV files to the root of the card, named SAMPLE1.WAV, SAMPLE2.WAV, SAMPLE3.WAV, and SAMPLE4.WAV.
7. Upload the firmware and power the board.

The firmware currently requires uncompressed PCM WAVs with a standard 44 byte header, 16 bit mono PCM and a sample rate of 22050 Hz. The current implementation of playback is blocking, meaning that the main control loop is not continually reading buttons while a sample is playing.

## Bill of Materials (BOM)

| Part | Purpose | Qty | Unit Price (CAD) | Total (CAD) | Vendor |
|---|---|---:|---:|---:|---|
| OLED 128x64 | Displays device status | 1 | $2.73 | $2.73 | AliExpress |
| 6-Pack Potentiometers | Control music parameters | 1 | $7.11 | $7.11 | Amazon |
| Momentary Push Buttons (10-Pack) | Control music playback and basic functions | 1 | $6.40 | $6.40 | Amazon |
| CD74HC4067 16-Channel Analog Multiplexer | Expands available analog inputs | 1 | $1.28 | $1.28 | AliExpress |
| 2-Way Switch (5-Pack) | Turns the device on and off | 1 | $2.55 | $2.55 | AliExpress |
| USB-C Input Port (5-Pack) | Provides a USB-C power input | 1 | $3.20 | $3.20 | AliExpress |
| MAX98357 Audio Amplifier | Amplifies audio for the speaker | 1 | $2.00 | $2.00 | AliExpress |
| MicroSD Storage Expansion Board | Reads audio files from a microSD card over SPI | 1 | $1.53 | $1.53 | AliExpress |
| Silicone Wire (24 AWG) | Connects electronic components | 1 | $2.85 | $2.85 | AliExpress |
| **Total** | | | | **$29.65** | |

The following table lists the hardware required to build the project. Prices are placeholders and should be updated to reflect the supplier, quantity, and currency you use.

# Safety and Compatibility Notes

- Do not apply 5V to Pico W GPIO pins. GPIO signals must remain within the board's supported 3.3V logic range.
- Verify the microSD module's supply voltage and logic-level compatibility before connecting it.
- Confirm that the speaker's impedance and power ratings are appropriate for the MAX98357A module.
- Avoid connecting multiple power sources in a way that can backfeed the USB supply.
- Test each module individually before assembling the full project.

# Future Improvements

- Add real time volume control while audio is playing.
- Assign different functions to the six potentiometers, such as volume, pitch, filtering, and effects.
- Add sample selection and playback indicatiors to the OLED interface.
