#ifndef SENSOR_H
#define SENSOR_H

#include <Arduino.h>
#include <DallasTemperature.h>
#include <OneWire.h>

class Sensor final {
  OneWire m_OneWire;
  DallasTemperature m_Sensor;
  uint8_t m_SensorAddress;

  void getSensorAddress();

public:
  explicit Sensor(uint8_t pin);

  float getTempC();

  static bool isValid(float temp_c);
};

#endif //SENSOR_H
