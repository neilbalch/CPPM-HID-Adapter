#include <CPPM.h>
#include "Joystick.h"

// Debug messages?
const bool DEBUG = true;

// Create Joystick
Joystick_ joystick;

// Object to store read CPPM values
struct CPPMFrame {
  // Analog sticks (values -1000 to 1000)
  int pitch = 0;
  int roll = 0;
  int thr = 0;
  int yaw = 0;

  // Switches/dials (values -1000 to 1000, usually -1000, 0, or 1000)
  int aux1 = 0;
  int aux2 = 0;
  int aux3 = 0;
  int aux4 = 0;
};

// Control output of debug messages
void sendDebugMsg(String message) {
  if(DEBUG) Serial.println(message);
}

// Returns true if CPPM is synchronised, false if it isn't.
// TODO(Neil): make sure this method sets values -1000 to 1000, use map if necessary
bool readCPPM(CPPMFrame* frame) {
  if(CPPM.synchronized()) {
    // TODO(Neil): make sure these are mapped to the right channels
    frame->roll = 2 * (CPPM.read_us(CPPM_AILE) - 1500);
    frame->pitch = 2 * (CPPM.read_us(CPPM_ELEV) - 1500);
    frame->thr = 2 * (CPPM.read_us(CPPM_THRO) - 1500);
    frame->yaw = 2 * CPPM.read_us(CPPM_RUDD) - 1500);
    frame->aux1 = 2 * CPPM.read_us(CPPM_GEAR) - 1500);
    frame->aux2 = 2 * CPPM.read_us(CPPM_AUX1) - 1500);
    frame->aux3 = 2 * CPPM.read_us(CPPM_AUX2) - 1500);
    frame->aux4 = 2 * CPPM.read_us(CPPM_AUX3) - 1500);

    return true;
  } else {
    return false;
  }
}

// Send joystick data from provided CPPMFrame to USB HID device
void sendJoystickData(CPPMFrame* frame) {
  joystick.setXAxis(frame->thr);
  joystick.setYAxis(frame->pitch);
  joystick.setRxAxis(frame->yaw);
  joystick.setRyAxis(frame->roll);

  joystick.setButton(0, frame->aux1);
  joystick.setButton(1, frame->aux2);
  joystick.setButton(2, frame->aux3);
  joystick.setButton(3, frame->aux4);
}

void setup() {
  Serial.begin(9600);
  CPPM.begin();
  sendDebugMsg("CPPM initialized");

  // Set joystick ranges to RC standard (1000-2000us --> +/- 1000)
  joystick.setXAxisRange(-1000, 1000);  // Throttle
  joystick.setYAxisRange(-1000, 1000);  // Pitch
  joystick.setRxAxisRange(-1000, 1000); // Yaw
  joystick.setRyAxisRange(-1000, 1000); // Roll
  sendDebugMsg("Joystick ranges set");

  // Start Joystick Emulation
  joystick.begin();
  sendDebugMsg("Joystick initialized");
}

void loop() {
  CPPMFrame frame;

  // Read newest CPPM frame
  if(!readCPPM(&frame)) {
    Serial.println("Err: CPPM signal not synchronised!");
  } else {
    sendDebugMsg("New CPPM frame read");
    sendJoystickData(&frame);
    sendDebugMsg("New CPPM frame sent to Joystick HID");
  }

  // Sleep for 50ms
  delay(50);
}