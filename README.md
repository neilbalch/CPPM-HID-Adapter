# CPPM-HID-Adapter

## Description

USB Joystick HID adapter for CPPM RC recievers. (*requires microprocesor with native USB support, e.g. Arduino Pro Micro or Arduino Duemilanove*) Dependent upon [`jmparatte/jm_CPPM`](https://github.com/jmparatte/jm_CPPM) and [`MHeironimus/ArduinoJoystickLibrary`](https://github.com/MHeironimus/ArduinoJoystickLibrary) for CPPM reading and Joystick HID support respectively.

This project makes use of the [PlatformIO](https://platformio.org) extension to VSCode for compilation and upload.

## Repository Map

* `.vscode/*`: Visual Studio Code workspace config files
* `Arduino_old`: Old project files from the Arduino IDE
  * `Debug-CPPM-Monitor.ino`: Debug program to test functionality of CPPM reading library
  * `Debug-JoystickTest.ino`: Debug program to test functionality of Joystick library
  * `Joystick.ino`: Main program, upload this to Arduino

## Hardware Setup

CPPM reader detects signals on Arduino pin 4. (*silkscreen lettering on Arduino Pro Micro*) Most RC RXs can be powered straight off the board's VCC rail. Pretty much just that; make sure that the correct wires go to the correct places.
