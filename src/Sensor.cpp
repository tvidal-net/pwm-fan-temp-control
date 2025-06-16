#include "Sensor.h"

#define SENSOR_INDEX   0x00

#define INVALID_TEMP_C DEVICE_DISCONNECTED_C;

Sensor::Sensor(const uint8_t pin, const Led* led) :
  m_Led(*led),
  m_OneWire(pin),
  m_Sensor(&m_OneWire) {
  m_Sensor.begin();
}

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
