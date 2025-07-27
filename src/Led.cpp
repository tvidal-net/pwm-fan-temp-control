#include <Arduino.h>

#include "Led.h"

#define BLINK_DELAY 100

Led::Led(const uint8_t pin) :
  m_Pin(pin) {
  pinMode(pin, OUTPUT);
  off();
}

bool Led::read() const {
  return !digitalRead(m_Pin);
}

void Led::off() const {
  digitalWrite(m_Pin, HIGH);
}

void Led::on() const {
  digitalWrite(m_Pin, LOW);
}

void Led::blink(const uint8_t times) const {
  for (uint8_t i = 0; i < times; ++i) {
    on();
    delay(BLINK_DELAY);
    off();
    delay(BLINK_DELAY);
  }
}
