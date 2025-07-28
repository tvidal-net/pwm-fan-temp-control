#include "Sensor.h"

#define RETRY_COUNT                          0x05
#define SENSOR_INDEX                         0x00

void Sensor::printStatus(const float temp_c) const {
  Serial.printf("TEMP:%02d,", m_Pin);
  Serial.printf("ADDR:%#04x,", m_SensorAddress[0]);
  Serial.printf("value=%02.1f°C\n", temp_c);
}

bool Sensor::getSensorAddress() {
  uint8_t retries = RETRY_COUNT;
  bool valid_address = false;
  while (retries-- && !((valid_address = m_Sensor.validAddress(m_SensorAddress)))) {
    if (m_Sensor.getAddress(m_SensorAddress, SENSOR_INDEX)) {
      Serial.print("SENSOR:0x");
      for (auto i = sizeof(m_SensorAddress); i--;) {
        Serial.printf("%02x", m_SensorAddress[i]);
      }
      Serial.println();
      return true;
    }
    delay(50);
  }
  return valid_address;
}

Sensor::Sensor(const uint8_t pin) :
  m_Pin(pin),
  m_OneWire(pin),
  m_Sensor(&m_OneWire) {
  m_Sensor.setWaitForConversion(true);
  m_Sensor.begin();
}

float Sensor::getTempC() {
  getSensorAddress();
  if (m_Sensor.validAddress(m_SensorAddress)) {
    const auto [ok, _] = m_Sensor.requestTemperatures();
    if (ok) {
      const float temp_c = m_Sensor.getTempC(m_SensorAddress, RETRY_COUNT);
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
