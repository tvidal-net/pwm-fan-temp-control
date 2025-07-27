#include "Sensor.h"

#define RETRY_COUNT                          0x05
#define SENSOR_INDEX                         0x00

void Sensor::printStatus(const float temp_c) const {
  Serial.printf("TEMP:%02d,", m_Pin);
  Serial.printf("ADDR:%#04x,", m_SensorAddress);
  Serial.printf("value=%02.1f°C\n", temp_c);
}

bool Sensor::hasValidAddress() const {
  return m_SensorAddress > 0x00 && m_SensorAddress < 0x7F;
}

void Sensor::getSensorAddress() {
  uint8_t retries = RETRY_COUNT;
  while (retries-- && !hasValidAddress() && !m_Sensor.getAddress(&m_SensorAddress, SENSOR_INDEX)) {
    m_Sensor.setWaitForConversion(true);
    m_Sensor.begin();
    delay(50);
  }
}

Sensor::Sensor(const uint8_t pin) :
  m_Pin(pin),
  m_OneWire(pin),
  m_Sensor(&m_OneWire) {}

float Sensor::getTempC() {
  getSensorAddress();
  if (m_SensorAddress) {
    const auto [ok, _] = m_Sensor.requestTemperaturesByAddress(&m_SensorAddress);
    if (ok) {
      const float temp_c = m_Sensor.getTempC(&m_SensorAddress, RETRY_COUNT);
      printStatus(temp_c);
      return temp_c;
    }
  }
  printStatus();
  return DEVICE_DISCONNECTED_C;
}

bool Sensor::isValid(const float temp_c) {
  return temp_c > DEVICE_DISCONNECTED_C;
}
