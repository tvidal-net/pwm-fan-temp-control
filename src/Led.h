#ifndef LED_H
#define LED_H

#include <Arduino.h>

class Led final {
  const uint8_t m_Pin;

  void printStatus(bool state) const;

public:
  explicit Led(uint8_t pin);

  bool read() const;

  void off() const;

  void on() const;

  void blink(uint8_t times = 5) const;
};

#endif //LED_H
