#include "Led.h"

#define BLINK_DELAY 100

void Led::printStatus(const bool state) const {
  Serial.printf("LED:%02d,", m_Pin);
  Serial.printf("%s,", read() ? "ON" : "OFF");
  Serial.printf("value=%d\n", state);
}

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
  printStatus(false);
}

void Led::on() const {
  digitalWrite(m_Pin, LOW);
  printStatus(true);
}

void Led::blink(const uint8_t times) const {
  for (uint8_t i = 0; i < times; ++i) {
    digitalWrite(m_Pin, LOW);
    delay(BLINK_DELAY);
    digitalWrite(m_Pin, HIGH);
    delay(BLINK_DELAY);
  }
}
