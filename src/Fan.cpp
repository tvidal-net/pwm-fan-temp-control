#include "Fan.h"

#define PWM_RANGE 1024
#define PWM_FREQ 1000

Fan::Fan(const uint8_t sw_pin, const uint8_t pwm_pin) :
  m_SW(sw_pin),
  m_PWM(pwm_pin) {
  pinMode(m_SW, OUTPUT);
  pinMode(m_PWM, OUTPUT);
  analogWriteRange(PWM_RANGE);
  analogWriteFreq(PWM_FREQ);
}

bool Fan::read() const {
  return digitalRead(m_SW);
}

void Fan::off() const {
  digitalWrite(m_SW, LOW);
}

void Fan::write(const float value) const {
  digitalWrite(m_SW, HIGH);

  const uint16_t duty = constrain(value * PWM_RANGE, 0, PWM_RANGE - 1);
  analogWrite(m_PWM, duty);
}
