#include "Fan.h"

#define PWM_RANGE 0x400
#define PWM_FREQ   1000
#define PWM_MIN   0.01f
#define PWM_MAX   1.00f

Fan::Fan(const uint8_t sw_pin, const uint8_t pwm_pin) :
  m_SW(sw_pin),
  m_PWM(pwm_pin) {
  pinMode(m_SW, OUTPUT);
  pinMode(m_PWM, OUTPUT);
  analogWriteRange(PWM_RANGE);
  analogWriteFreq(PWM_FREQ);
  this->write(PWM_MAX);
}

bool Fan::read() const {
  return !digitalRead(m_SW);
}

void Fan::off() const {
  digitalWrite(m_SW, HIGH);
}

void Fan::write(const float pwm) const {
  digitalWrite(m_SW, LOW);

  const uint16_t duty = constrain(
    pwm * PWM_RANGE,
    PWM_MIN * PWM_RANGE,
    PWM_RANGE - 1
  );
  analogWrite(m_PWM, duty);
}
