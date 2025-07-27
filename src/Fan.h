#ifndef FAN_H
#define FAN_H

#include <Arduino.h>

#define PWM_OFF                   0.00f
#define PWM_MIN                   0.01f

class Fan final {
  const uint8_t m_SW;
  const uint8_t m_PWM;

  void printStatus(float pwm, uint16_t duty) const;

public:
  Fan(uint8_t sw_pin, uint8_t pwm_pin);

  bool read() const;

  void off() const;

  void write(float value) const;
};

#endif //FAN_H
