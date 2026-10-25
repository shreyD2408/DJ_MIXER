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
