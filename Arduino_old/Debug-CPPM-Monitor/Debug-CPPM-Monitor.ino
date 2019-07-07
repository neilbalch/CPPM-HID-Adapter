// CPPM reading demo slightly modified from CPPM library original sample code

#include <CPPM.h>

void setup() {
  Serial.begin(250000);
  CPPM.begin();
}

void loop() {
  if (CPPM.synchronized()) {
    int aile = CPPM.read_us(CPPM_AILE) - 1500;  // aile
    int elev = CPPM.read_us(CPPM_ELEV) - 1500;  // elevator
    int thro = CPPM.read_us(CPPM_THRO) - 1500;  // throttle
    int rudd = CPPM.read_us(CPPM_RUDD) - 1500;  // rudder
    int gear = CPPM.read_us(CPPM_GEAR) - 1500;  // gear
    int aux1 = CPPM.read_us(CPPM_AUX1) - 1500;  // flap
    int aux2 = CPPM.read_us(CPPM_AUX2) - 1500;  // aux2
    int aux3 = CPPM.read_us(CPPM_AUX3) - 1500;  // aux3

    Serial.print(aile);
    Serial.print(",\t");
    Serial.print(elev);
    Serial.print(",\t");
    Serial.print(thro);
    Serial.print(",\t");
    Serial.print(rudd);
    Serial.print(",\t");
    Serial.print(gear);
    Serial.print(",\t");
    Serial.print(aux1);
    Serial.print(",\t");
    Serial.print(aux2);
    Serial.print(",\t");
    Serial.print(aux3);
    Serial.print("\n");
    Serial.flush();
  } else {
    // if not synchronized, do something...
    Serial.println("Err: CPPM desync");
  }
}
