# Arduino-CPPM-Adapter

## Description

USB Joystick HID adapter for CPPM RC recievers. (*requires microprocesor with native USB support, e.g. Arduino Pro Micro or Arduino Duemilanove*) Dependent upon [`jmparatte/CPPM`](https://github.com/jmparatte/CPPM) and [`MHeironimus/ArduinoJoystickLibrary`](https://github.com/MHeironimus/ArduinoJoystickLibrary) for CPPM reading and Joystick HID support respectively.

## Repository Map

* `.vscode/*`: Visual Studio Code workspace config files
* `Debug-CPPM-Monitor.ino`: Debug program to test functionality of CPPM reading library
* `Joystick.ino`: Main program, upload this to Arduino

## Hardware Setup

CPPM reader detects signals on Arduino pin 8. (*silkscreen pin 9 on Arduino Uno*) Most RC RXs can be powered straight off the board's 5V rail. Pretty much just that; make sure that the correct wires go to the correct places.