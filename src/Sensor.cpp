//
// Created by thiago on 16/06/25.
//

#include "Sensor.h"

#include <Arduino.h>

#define LED                LED_BUILTIN

#define SENSOR_INDEX              0x00

Sensor::Sensor(const uint8_t pin) :
  oneWire(pin),
  sensor(&oneWire) {}

float Sensor::getTempC() {
  digitalWrite(LED, HIGH);
  sensor.requestTemperatures();

  const float temp_c = sensor.getTempCByIndex(SENSOR_INDEX);
  digitalWrite(LED, LOW);
  return temp_c;
}

bool Sensor::isValid(const float temp_c) {
  return temp_c > INVALID_TEMP_C;
}
