#include "Fan.h"

#define PWM_RANGE                 0x400
#define PWM_FREQ                   1000

void Fan::printStatus(const float pwm, const uint16_t duty) const {
  Serial.printf("FAN:%02d,", m_SW);
  Serial.printf("PWM:%02d,", m_PWM);
  Serial.printf("%s,", read() ? "ON" : "OFF");
  Serial.printf("duty=%#03x,", duty);
  Serial.printf("value=%03.1f%%\n", 100.0f * pwm);
}

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

void Fan::write(const float value) const {
  digitalWrite(m_SW, value ? LOW : HIGH);
  const uint16_t duty = constrain(
    value * PWM_RANGE,
    PWM_MIN * PWM_RANGE,
    PWM_RANGE
  );
  printStatus(value, duty);
  analogWrite(m_PWM, PWM_RANGE - duty);
}
