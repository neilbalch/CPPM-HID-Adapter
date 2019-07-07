#ifndef HELPERS_H
#define HELPERS_H

#include <Arduino.h>
#include <constants.h>

// Enum type for different log levels
enum LogLevel { DEBUG, CRITICAL };

// Control output of debug messages
void sendSerialMsg(LogLevel level, String message) {
  if (LOG_DEBUG && level == DEBUG)
    Serial.println((String)millis() + ": " + message);
  if (level == CRITICAL)
    Serial.println((String)millis() + ": CRITICAL! " + message);
}

#endif
