# Arduino-CPPM-Adapter

## Description

USB Joystick HID adapter for CPPM RC recievers. (*requires microprocesor with native USB support, e.g. Arduino Pro Micro or Arduino Duemilanove*) Dependent upon [`neilbalch/CPPM`](https://github.com/neilbalch/CPPM) (*specialty fork of [`jmparatte/CPPM`](https://github.com/jmparatte/CPPM)*) and [`MHeironimus/ArduinoJoystickLibrary`](https://github.com/MHeironimus/ArduinoJoystickLibrary) for CPPM reading and Joystick HID support respectively.

## Repository Map

* `.vscode/*`: Visual Studio Code workspace config files
* `Debug-CPPM-Monitor.ino`: Debug program to test functionality of CPPM reading library
* `Debug-JoystickTest.ino`: Debug program to test functionality of Joystick library
* `Joystick.ino`: Main program, upload this to Arduino

## Hardware Setup

CPPM reader detects signals on Arduino pin 4. (*silkscreen lettering on Arduino Pro Micro*) Most RC RXs can be powered straight off the board's VCC rail. Pretty much just that; make sure that the correct wires go to the correct places.