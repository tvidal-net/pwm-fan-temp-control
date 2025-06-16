#ifndef FAN_H
#define FAN_H

#include <Arduino.h>

class Fan final {
  uint8_t m_SW;
  uint8_t m_PWM;

public:
  explicit Fan(uint8_t sw_pin, uint8_t pwm_pin);

  bool read() const;
  void off() const;

  void write(float value) const;
};

#endif //FAN_H
