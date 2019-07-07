#ifndef CPPMFRAME_H
#define CPPMFRAME_H

#include <Arduino.h>
#include <constants.h>

// Stores one set of CPPM channel values
struct CPPMFrame {
 private:
  bool alreadyMapped = false;

 public:
  // Analog sticks (values -1000 to 1000)
  int pitch, roll, thr, yaw = 0;

  // Switches/dials (values -1000 to 1000, usually -1000, 0, or 1000)
  int aux1, aux2, aux3, aux4 = 0;

  // Represents member vars in an easily printable form
  String toString();

  // Remap data so it matches proper range for joystick
  void mapDataToJoystickRange();
};

#endif
