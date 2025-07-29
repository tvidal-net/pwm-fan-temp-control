#ifndef SENSOR_H
#define SENSOR_H

#include <Arduino.h>
#include <DallasTemperature.h>
#include <OneWire.h>

class Sensor final {
  const uint8_t m_Pin;
  OneWire m_OneWire;
  DallasTemperature m_Sensor;
  DeviceAddress m_SensorAddress;

  void printStatus(float temp_c) const;

  void reset();

  bool getSensorAddress();

public:
  explicit Sensor(uint8_t pin);

  float getTempC();

  static
  bool isValid(float temp_c);
};

#endif //SENSOR_H
