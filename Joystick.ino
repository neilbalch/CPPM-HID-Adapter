#include <CPPM.h>
#include <Joystick.h>

// Debug messages in serial monitor?
const bool LOG_DEBUG = true;
// Enum type for different log levels
enum LogLevel {DEBUG, CRITICAL};

// Create Joystick
Joystick_ joystick;
// Greatest possible magnitude of joystick axis
const int JOYSTICK_RANGE = 1000;

// Stores one set of CPPM channel values
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

  // Represents member vars in an easily printable form
  String toString() {
    return "roll:" + frame->roll +
    "\tpitch:" + frame->pitch +
    "\tthrottle:" + frame->throttle +
    "\tyaw:" + frame->yaw +
    "\taux1:" + frame->aux1 +
    "\taux2:" + frame->aux2 +
    "\taux3:" + frame->aux3 +
    "\taux4:" + frame->aux4;
  }

  // Remap data so it matches proper range for joystick
  void mapDataToJoystickRange() {
    map(pitch, -1000, 1000, -JOYSTICK_RANGE, JOYSTICK_RANGE);
    map(roll, -1000, 1000, -JOYSTICK_RANGE, JOYSTICK_RANGE);
    map(thr, -1000, 1000, -JOYSTICK_RANGE, JOYSTICK_RANGE);
    map(yaw, -1000, 1000, -JOYSTICK_RANGE, JOYSTICK_RANGE);
    map(aux1, -1000, 1000, -JOYSTICK_RANGE, JOYSTICK_RANGE);
    map(aux2, -1000, 1000, -JOYSTICK_RANGE, JOYSTICK_RANGE);
    map(aux3, -1000, 1000, -JOYSTICK_RANGE, JOYSTICK_RANGE);
    map(aux4, -1000, 1000, -JOYSTICK_RANGE, JOYSTICK_RANGE);
  }
};

// Control output of debug messages
void sendSerialMsg(LogLevel level, String message) {
  if(LOG_DEBUG && level == DEBUG) Serial.println((String)millis() + ": " + message);
  if(level = CRITICAL) Serial.println((String)millis() + ": CRITICAL! " + message);
}

// Returns true if CPPM is synchronised, false if it isn't.
bool readCPPM(CPPMFrame *frame) {
  if(CPPM.synchronized()) {
    // Values come in on a scale 1000us to 2000us, but we want them on a scale
    // of -1000us to 1000us
    frame->roll = 2 * (CPPM.read_us(CPPM_AILE) - 1500);
    frame->pitch = 2 * (CPPM.read_us(CPPM_ELEV) - 1500);
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
    return false;
  }
}

// Send joystick data from provided CPPMFrame to USB HID device
void sendJoystickData(CPPMFrame *frame) {
  // Map data to proper ranges
  frame->mapDataToJoystickRange();

  // Send analog sticks
  joystick.setXAxis(frame->thr);
  joystick.setYAxis(frame->pitch);
  joystick.setRxAxis(frame->yaw);
  joystick.setRyAxis(frame->roll);

  // Send buttons
  joystick.setButton(0, frame->aux1);
  joystick.setButton(1, frame->aux2);
  joystick.setButton(2, frame->aux3);
  joystick.setButton(3, frame->aux4);

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
  joystick.setXAxisRange(-JOYSTICK_RANGE, JOYSTICK_RANGE);  // Throttle
  joystick.setYAxisRange(-JOYSTICK_RANGE, JOYSTICK_RANGE);  // Pitch
  joystick.setRxAxisRange(-JOYSTICK_RANGE, JOYSTICK_RANGE); // Yaw
  joystick.setRyAxisRange(-JOYSTICK_RANGE, JOYSTICK_RANGE); // Roll
  sendSerialMsg(DEBUG, "Joystick ranges set");

  // Start Joystick Emulation
  joystick.begin();
  sendSerialMsg(DEBUG, "Joystick initialized");

  // Enable board-specific signal LED
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  CPPMFrame frame;

  // Read newest CPPM frame, detect desyncs
  if(!readCPPM(&frame)) {
    digitalWrite(LED_BUILTIN, HIGH);
    sendSerialMsg(CRITICAL, "CPPM signal not synchronised!");
  } else {
    digitalWrite(LED_BUILTIN, LOW);
    sendSerialMsg(DEBUG, "New CPPM frame read");

    sendJoystickData(&frame);
    sendSerialMsg(DEBUG, "New CPPM frame sent to Joystick HID");
  }

  // Sleep for 50ms
  // TODO(Neil): Speed this up as fast as possible, joystick must be fed ASAP!
  delay(50);
}