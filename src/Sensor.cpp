#include "Sensor.h"

#define RETRY_COUNT                          0x05
#define SENSOR_INDEX                         0x00

void Sensor::getSensorAddress() {
  uint8_t retries = RETRY_COUNT;
  while (retries-- && !m_SensorAddress && !m_Sensor.getAddress(&m_SensorAddress, SENSOR_INDEX)) {
    m_Sensor.setWaitForConversion(true);
    m_Sensor.begin();
    delay(50);
  }
}

Sensor::Sensor(const uint8_t pin) :
  m_OneWire(pin),
  m_Sensor(&m_OneWire) {}

float Sensor::getTempC() {
  this->getSensorAddress();
  if (m_SensorAddress) {
    const auto [ok, _] = m_Sensor.requestTemperaturesByAddress(&m_SensorAddress);
    if (ok) {
      const float temp_c = m_Sensor.getTempC(&m_SensorAddress, RETRY_COUNT);
      return temp_c;
    }
  }
  return DEVICE_DISCONNECTED_C;
}

bool Sensor::isValid(const float temp_c) {
  return temp_c > DEVICE_DISCONNECTED_C;
}
