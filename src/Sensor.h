#ifndef SENSOR_H
#define SENSOR_H

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include "Led.h"

class Sensor final {
  constexpr static float INVALID_TEMP_C = DEVICE_DISCONNECTED_C;

  const Led m_Led;
  OneWire m_OneWire;
  DallasTemperature m_Sensor;

public:
  explicit Sensor(uint8_t pin);

  float getTempC();

  static bool isValid(float temp_c);
};

#endif //SENSOR_H
