//
// Created by thiago on 16/06/25.
//

#ifndef SENSOR_H
#define SENSOR_H

#include <OneWire.h>
#include <DallasTemperature.h>

class Sensor {
  constexpr static float INVALID_TEMP_C = DEVICE_DISCONNECTED_C;

  OneWire oneWire;
  DallasTemperature sensor;

public:
  explicit Sensor(uint8_t pin);

  float getTempC();

  static bool isValid(float temp_c);
};

#endif //SENSOR_H
