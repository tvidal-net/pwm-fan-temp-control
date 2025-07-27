#include "Fan.h"

#define PWM_RANGE                 0x400
#define PWM_FREQ                   1000

Fan::Fan(const uint8_t sw_pin, const uint8_t pwm_pin) :
  m_SW(sw_pin),
  m_PWM(pwm_pin) {
  pinMode(m_SW, OUTPUT);
  pinMode(m_PWM, OUTPUT);

  analogWriteRange(PWM_RANGE);
  analogWriteFreq(PWM_FREQ);

  off();
}

bool Fan::read() const {
  return !digitalRead(m_SW);
}

void Fan::off() const {
  write(PWM_OFF);
}

void Fan::write(const float pwm) const {
  digitalWrite(m_SW, pwm ? LOW : HIGH);
  const uint16_t duty = constrain(
    pwm * PWM_RANGE,
    PWM_MIN * PWM_RANGE,
    PWM_RANGE
  );
  analogWrite(m_PWM, PWM_RANGE - duty);
}
