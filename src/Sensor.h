#ifndef SENSOR_H
#define SENSOR_H

#include <Arduino.h>
#include <DallasTemperature.h>
#include <OneWire.h>

#include "Led.h"

class Sensor final {
  const Led& m_Led;
  OneWire m_OneWire;
  DallasTemperature m_Sensor;

public:
  explicit Sensor(uint8_t pin, const Led* led);

  float getTempC();

  static bool isValid(float temp_c);
};

#endif //SENSOR_H
