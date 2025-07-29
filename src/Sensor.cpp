#include "Sensor.h"

#define RETRY_COUNT                          0x05
#define SENSOR_INDEX                         0x00

#define MIN_TEMP                            00.0f
#define MAX_TEMP                            77.0f

#define FAILED_REQUEST                      -2.0f
#define FAILED_ADDRESS                      -1.0f

static
void print_address(const DeviceAddress& address) {
  Serial.print("SENSOR:0x");
  for (auto i = sizeof(address); i--;) {
    Serial.printf("%02x", address[i]);
  }
  Serial.println();
}

void Sensor::printStatus(const float temp_c) const {
  Serial.printf("TEMP:%02d,", m_Pin);
  Serial.printf("ADDR:%#04x,", m_SensorAddress[0]);
  Serial.printf("value=%.1f°C\n", temp_c);
}

void Sensor::reset() {
  memset(m_SensorAddress, 0xFF, sizeof(m_SensorAddress));
  m_Sensor.setWaitForConversion(true);
  m_Sensor.begin();
}

bool Sensor::getSensorAddress() {
  uint8_t retries = RETRY_COUNT;
  bool valid_address = false;
  while (retries-- && !((valid_address = m_Sensor.validAddress(m_SensorAddress)))) {
    Serial.printf("SENSOR:%02d,r=%d\n", m_Pin, retries);
    if (m_Sensor.getAddress(m_SensorAddress, SENSOR_INDEX)) {
      print_address(m_SensorAddress);
      return true;
    }
    delay(200);
  }
  return valid_address;
}

Sensor::Sensor(const uint8_t pin) :
  m_Pin(pin),
  m_OneWire(pin),
  m_Sensor(&m_OneWire) {
  reset();
}

float Sensor::getTempC() {
  if (getSensorAddress()) {
    const auto [ok, _] = m_Sensor.requestTemperatures();
    if (ok) {
      const float temp_c = m_Sensor.getTempC(m_SensorAddress, RETRY_COUNT);
      printStatus(temp_c);
      return temp_c;
    }
    printStatus(FAILED_REQUEST);
    reset();
  }
  printStatus(FAILED_ADDRESS);
  print_address(m_SensorAddress);
  return DEVICE_DISCONNECTED_C;
}

bool Sensor::isValid(const float temp_c) {
  return temp_c > MIN_TEMP && temp_c < MAX_TEMP;
}
