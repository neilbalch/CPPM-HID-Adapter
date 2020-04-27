#include <Arduino.h>
#include <CPPMFrame.h>
#include <Joystick.h>
#include <constants.h>
#include <helpers.h>
#include <jm_CPPM.h>

// Create Joystick
Joystick_ joystick;

// Storage for previous frame of CPPM values
CPPMFrame previousFrame;

// Returns true if CPPM is synchronised, false if it isn't.
bool readCPPM(CPPMFrame *frame) {
  // Update class vars and check timeouts
  CPPM.cycle();

  if (CPPM.synchronized()) {
    // Values come in on a scale of 1000µs to 2000µs, but we want them on a
    // scale of -1000us to 1000
    frame->roll = constrain((CPPM.read_us(CPPM_AILE) - 1000), 0, 1000);
    frame->pitch = constrain((CPPM.read_us(CPPM_ELEV) - 1000), 0, 5000);
    frame->thr = constrain((CPPM.read_us(CPPM_THRO) - 1000), 0, 1000);
    frame->yaw = constrain((CPPM.read_us(CPPM_RUDD) - 1000), 0, 1000);
    frame->aux1 = constrain((CPPM.read_us(CPPM_GEAR) - 1000), 0, 1000);
    frame->aux2 = constrain((CPPM.read_us(CPPM_AUX1) - 1000), 0, 1000);
    frame->aux3 = constrain((CPPM.read_us(CPPM_AUX2) - 1000), 0, 1000);
    frame->aux4 = constrain((CPPM.read_us(CPPM_AUX3) - 1000), 0, 1000);

    // Log read data
    sendSerialMsg(DEBUG, "CPPM Read: " + frame->toString());

    return true;
  } else {
    sendSerialMsg(CRITICAL, "No CPPM read due to desync");

    frame->roll = previousFrame.roll;
    frame->pitch = previousFrame.pitch;
    frame->thr = previousFrame.thr;
    frame->yaw = previousFrame.yaw;
    frame->aux1 = previousFrame.aux1;
    frame->aux2 = previousFrame.aux2;
    frame->aux3 = previousFrame.aux3;
    frame->aux4 = previousFrame.aux4;

    return false;
  }
}

// Send joystick data from provided CPPMFrame to USB HID device
void sendJoystickData(CPPMFrame *frame) {
  // Map data to proper ranges
  frame->mapDataToJoystickRange();

  // Send analog sticks
  joystick.setXAxis(frame->roll);
  joystick.setYAxis(frame->pitch);
  joystick.setRxAxis(frame->yaw);
  joystick.setZAxis(frame->thr);

  // Send aux channels
  // TODO(Neil): Figure out how to submit the rest of the values...
  if (!sendButttonsToAuxChannels) {
    joystick.setRzAxis(frame->aux1);
    joystick.setRyAxis(frame->aux2);
  } else {
    if (frame->aux1 < 0) {
      joystick.setButton(0, 1);
      joystick.setButton(1, 0);
      joystick.setButton(2, 0);
    } else if (frame->aux1 > -5 && frame->aux1 < 5) {
      joystick.setButton(0, 0);
      joystick.setButton(1, 1);
      joystick.setButton(2, 0);
    } else {
      joystick.setButton(0, 0);
      joystick.setButton(1, 0);
      joystick.setButton(2, 1);
    }

    if (frame->aux2 < 0) {
      joystick.setButton(3, 1);
      joystick.setButton(4, 0);
      joystick.setButton(5, 0);
    } else if (frame->aux2 > -5 && frame->aux2 < 5) {
      joystick.setButton(3, 0);
      joystick.setButton(4, 1);
      joystick.setButton(5, 0);
      ;
    } else {
      joystick.setButton(3, 0);
      joystick.setButton(4, 0);
      joystick.setButton(5, 1);
      ;
    }

    if (frame->aux3 < 0) {
      joystick.setButton(6, 1);
      joystick.setButton(7, 0);
      joystick.setButton(8, 0);
    } else if (frame->aux3 > -5 && frame->aux3 < 5) {
      joystick.setButton(6, 0);
      joystick.setButton(7, 1);
      joystick.setButton(8, 0);
    } else {
      joystick.setButton(6, 0);
      joystick.setButton(7, 0);
      joystick.setButton(8, 1);
    }

    if (frame->aux4 < 0) {
      joystick.setButton(9, 1);
      joystick.setButton(10, 0);
      joystick.setButton(11, 0);
    } else if (frame->aux4 > -5 && frame->aux4 < 5) {
      joystick.setButton(9, 0);
      joystick.setButton(10, 1);
      joystick.setButton(11, 0);
    } else {
      joystick.setButton(9, 0);
      joystick.setButton(10, 0);
      joystick.setButton(11, 1);
    }
  }

  // Log sent data
  // sendSerialMsg(DEBUG, "Joystick Data Sent: " + frame->toString());
}

void setup() {
  // Init serial monitor
  Serial.begin(9600);

  // Init CPPM reader
  CPPM.begin();
  sendSerialMsg(DEBUG, "CPPM reader initialized");

  // Set joystick ranges
  joystick.setZAxisRange(-JOYSTICK_RANGE, JOYSTICK_RANGE);   // Throttle
  joystick.setXAxisRange(-JOYSTICK_RANGE, JOYSTICK_RANGE);   // Roll
  joystick.setYAxisRange(-JOYSTICK_RANGE, JOYSTICK_RANGE);   // Pitch
  joystick.setRxAxisRange(-JOYSTICK_RANGE, JOYSTICK_RANGE);  // Yaw

  // Set aux channel ranges, if applicable
  if (!sendButttonsToAuxChannels) {
    joystick.setRzAxisRange(-JOYSTICK_RANGE, JOYSTICK_RANGE);  // Aux1
    joystick.setRyAxisRange(-JOYSTICK_RANGE, JOYSTICK_RANGE);  // Aux2
  }
  sendSerialMsg(DEBUG, "Joystick ranges set");

  // Start Joystick Emulation
  joystick.begin();
  sendSerialMsg(DEBUG, "Joystick initialized");
}

void loop() {
  CPPMFrame frame;

  // Read newest CPPM frame, detect desyncs
  if (!readCPPM(&frame)) {
    sendSerialMsg(CRITICAL, "CPPM signal not synchronised!");

    sendJoystickData(&frame);
  } else {
    // sendSerialMsg(DEBUG, "New CPPM frame read");

    sendJoystickData(&frame);
    // sendSerialMsg(DEBUG, "New CPPM frame sent to Joystick HID");
  }

  previousFrame = frame;
}
