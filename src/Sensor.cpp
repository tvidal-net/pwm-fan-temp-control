#include "Sensor.h"

#include <Arduino.h>

#define LED LED_BUILTIN
#define SENSOR_INDEX 0x00

Sensor::Sensor(const uint8_t pin) :
  m_OneWire(pin),
  m_Sensor(&m_OneWire) {}

float Sensor::getTempC() {
  m_Led.high();
  m_Sensor.requestTemperatures();

  const float temp_c = m_Sensor.getTempCByIndex(SENSOR_INDEX);
  m_Led.low();
  return temp_c;
}

bool Sensor::isValid(const float temp_c) {
  return temp_c > INVALID_TEMP_C;
}
