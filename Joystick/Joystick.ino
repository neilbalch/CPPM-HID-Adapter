#include <jm_CPPM.h>
#include <Joystick.h>

//////////////////////////////////////////////////\
//                  Constants                     |
//////////////////////////////////////////////////|

// Debug messages in serial monitor?
const bool LOG_DEBUG = false;

// Greatest possible magnitude of joystick axis
const int JOYSTICK_RANGE = 1000;
// Send aux channels to HID buttons or HID axes?
const bool sendButttonsToAuxChannels = false;

///////////////////////////////////////////////////

// Enum type for different log levels
enum LogLevel { DEBUG, CRITICAL };

// Create Joystick
Joystick_ joystick;

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
  String toString() {
    return "roll:" + (String)roll + "\tpitch:" + (String)pitch +
           "\tthrottle:" + (String)thr + "\tyaw:" + (String)yaw +
           "\taux1:" + (String)aux1 + "\taux2:" + (String)aux2 +
           "\taux3:" + (String)aux3 + "\taux4:" + (String)aux4;
  }

  // Remap data so it matches proper range for joystick
  void mapDataToJoystickRange() {
    if (!alreadyMapped) {
      // map-em
      map(pitch, -1000, 1000, -JOYSTICK_RANGE, JOYSTICK_RANGE);
      map(roll, -1000, 1000, -JOYSTICK_RANGE, JOYSTICK_RANGE);
      map(thr, -1000, 1000, -JOYSTICK_RANGE, JOYSTICK_RANGE);
      map(yaw, -1000, 1000, -JOYSTICK_RANGE, JOYSTICK_RANGE);
      map(aux1, -1000, 1000, -JOYSTICK_RANGE, JOYSTICK_RANGE);
      map(aux2, -1000, 1000, -JOYSTICK_RANGE, JOYSTICK_RANGE);
      map(aux3, -1000, 1000, -JOYSTICK_RANGE, JOYSTICK_RANGE);
      map(aux4, -1000, 1000, -JOYSTICK_RANGE, JOYSTICK_RANGE);

      // Make doubly sure they're within the right range
      constrain(pitch, -JOYSTICK_RANGE, JOYSTICK_RANGE);
      constrain(roll, -JOYSTICK_RANGE, JOYSTICK_RANGE);
      constrain(thr, -JOYSTICK_RANGE, JOYSTICK_RANGE);
      constrain(yaw, -JOYSTICK_RANGE, JOYSTICK_RANGE);
      constrain(aux1, -JOYSTICK_RANGE, JOYSTICK_RANGE);
      constrain(aux2, -JOYSTICK_RANGE, JOYSTICK_RANGE);
      constrain(aux3, -JOYSTICK_RANGE, JOYSTICK_RANGE);
      constrain(aux4, -JOYSTICK_RANGE, JOYSTICK_RANGE);
      alreadyMapped = true;
    }
  }
};

// Storage for previous frame of CPPM values
CPPMFrame previousFrame;

// Control output of debug messages
void sendSerialMsg(LogLevel level, String message) {
  if (LOG_DEBUG && level == DEBUG)
    Serial.println((String)millis() + ": " + message);
  if (level == CRITICAL)
    Serial.println((String)millis() + ": CRITICAL! " + message);
}

// Returns true if CPPM is synchronised, false if it isn't.
bool readCPPM(CPPMFrame *frame) {
  // Update class vars and check timeouts
  CPPM.cycle();

  if (CPPM.synchronized()) {
    // Values come in on a scale of 1000µs to 2000µs, but we want them on a
    // scale of -1000us to 1000
    frame->roll = 2 * (CPPM.read_us(CPPM_AILE) - 1500);
    frame->pitch = -2 * (CPPM.read_us(CPPM_ELEV) - 1500);  // Requires Inversion
    frame->thr = 2 * (CPPM.read_us(CPPM_THRO) - 1500);
    frame->yaw = 2 * (CPPM.read_us(CPPM_RUDD) - 1500);
    frame->aux1 = 2 * (CPPM.read_us(CPPM_GEAR) - 1500);
    frame->aux2 = 2 * (CPPM.read_us(CPPM_AUX1) - 1500);
    frame->aux3 = 2 * (CPPM.read_us(CPPM_AUX2) - 1500);
    frame->aux4 = 2 * (CPPM.read_us(CPPM_AUX3) - 1500);

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
  //TODO(Neil): Figure out how to submit the rest of the values...
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
  sendSerialMsg(DEBUG, "Joystick Data Sent: " + frame->toString());
}

void setup() {
  // Init serial monitor
  Serial.begin(9600);

  // Init CPPM reader
  CPPM.begin();
  sendSerialMsg(DEBUG, "CPPM reader initialized");

  // Set joystick ranges
  joystick.setZAxisRange(-JOYSTICK_RANGE, JOYSTICK_RANGE);     // Throttle
  joystick.setXAxisRange(-JOYSTICK_RANGE, JOYSTICK_RANGE);     // Roll
  joystick.setYAxisRange(-JOYSTICK_RANGE, JOYSTICK_RANGE);     // Pitch
  joystick.setRxAxisRange(-JOYSTICK_RANGE, JOYSTICK_RANGE);    // Yaw

  // Set aux channel ranges, if applicable
  if (!sendButttonsToAuxChannels) {
    joystick.setRzAxisRange(-JOYSTICK_RANGE, JOYSTICK_RANGE);    // Aux1
    joystick.setRyAxisRange(-JOYSTICK_RANGE, JOYSTICK_RANGE);    // Aux2
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
    sendSerialMsg(DEBUG, "New CPPM frame read");

    sendJoystickData(&frame);
    sendSerialMsg(DEBUG, "New CPPM frame sent to Joystick HID");
  }

  previousFrame = frame;
}
