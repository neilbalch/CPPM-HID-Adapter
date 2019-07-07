#include <CPPMFrame.h>

String CPPMFrame::toString() {
  return "roll:" + (String)roll + "\tpitch:" + (String)pitch +
          "\tthrottle:" + (String)thr + "\tyaw:" + (String)yaw +
          "\taux1:" + (String)aux1 + "\taux2:" + (String)aux2 +
          "\taux3:" + (String)aux3 + "\taux4:" + (String)aux4;
}

void CPPMFrame::mapDataToJoystickRange() {
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

    alreadyMapped = true;
  }
}