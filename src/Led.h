#ifndef LED_H
#define LED_H

#include <Arduino.h>

class Led final {
  const uint8_t m_Pin;

public:
  explicit Led(uint8_t pin = LED_BUILTIN, uint8_t pin_mode = OUTPUT);

  bool read() const;

  void off() const;

  void on() const;

  void blink(uint8_t times = 5) const;
};

#endif //LED_H
