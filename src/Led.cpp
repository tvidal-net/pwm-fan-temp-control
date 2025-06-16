#include "Led.h"

#define BLINK_DELAY 100

Led::Led(const uint8_t pin, const uint8_t pinMode) : m_Pin(pin) {
  pinMode(pin, pinMode);
}

bool Led::read() const {
  return digitalRead(m_Pin);
}

void Led::high() const {
  digitalWrite(m_Pin, HIGH);
}

void Led::low() const {
  digitalWrite(m_Pin, LOW);
}

void Led::blink(const uint8_t times) const {
  for (uint8_t i = 0; i < times; ++i) {
    high();
    delay(BLINK_DELAY);
    low();
    delay(BLINK_DELAY);
  }
}
