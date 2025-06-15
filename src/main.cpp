// ***************************************************
// ** pwm-fan-temp-control/src/main.cpp
// ***************************************************
// vim: ts=2 sw=2 et:

#include <Arduino.h>
#include <avr/wdt.h>
#include <OneWire.h>
#include <SimpleTimer.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS        PIN2
#define FAN_PWM             PIN3
#define FAN_SW              PIN4

#define FAN_PWM_HIGH        79

#define TIMER_INTERVAL      2000
#define BLINK_DELAY         50

#define PWM_FAN_MIN         0.01f
#define PWM_FAN_MAX         1.00f

#define TEMP_COUNT          6
#define TEMP_OFF            30.0f
#define TEMP_ON             40.0f
#define TEMP_LIMIT          60.0f
#define TEMP_CALIBRATION    1.00f

#define IS_TEMP_READING     ((bool)digitalRead(LED_BUILTIN))
#define TEMP_AVG            (temp_sum / TEMP_COUNT)
#define FAN_READ            ((bool)digitalRead(FAN_SW))

#define SET_FAN_PWM(pwm)    OCR2B = (uint8_t)(FAN_PWM_HIGH*(pwm+PWM_FAN_MIN))

const char* ON = "ON";
const char* OFF = "OFF";

OneWire one_wire_bus(ONE_WIRE_BUS);
DallasTemperature temp_sensor(&one_wire_bus);

SimpleTimer timer(TIMER_INTERVAL);

uint8_t temp_index = 0;
float temp_sum = 0.0f;
float temp_data[TEMP_COUNT];

float fan_pwm = PWM_FAN_MIN;

void printTempData(const float temp_avg) {
  Serial.print("[");
  Serial.print(temp_data[0], 1);
  for (uint8_t i = 1; i < TEMP_COUNT; i++) {
    Serial.print(",");
    Serial.print(temp_data[i], 1);
  }
  Serial.print("] avg=");
  Serial.print(temp_avg, 1);
}

void printTempReadError(const float temp) {
#ifndef RELEASE
  Serial.print("# [");
  Serial.print(temp, 1);
  Serial.print("] - ERROR !!!");
  Serial.println();
#endif
}

void printFanStatus(const bool fan_status) {
  if (fan_status) {
    Serial.print(" fan=");
    Serial.print(fan_pwm * 100, 1);
    Serial.print("%");
  }
}

void printStatus() {
#ifndef RELEASE
  Serial.print("- ");
  printTempData(TEMP_AVG);
  printFanStatus(FAN_READ);
  Serial.println();
#endif
}

void resetTempSensor() {
  temp_sensor.begin();
  temp_sensor.setWaitForConversion(false);
}

void setupSerial() {
#ifndef RELEASE
  Serial.begin(9600);
  Serial.println("\n\npwm-fan-temp-control");
#endif
}

void setupTimerPWM() {
  TIMSK2 = TIFR2 = 0;
  TCCR2A = (1 << COM2B1) | (1 << WGM21) | (1 << WGM20);
  TCCR2B = (1 << WGM22) | (1 << CS21);
  OCR2A = FAN_PWM_HIGH;
  OCR2B = 0;
}

void setupFanControl() {
  // pwm
  pinMode(FAN_PWM, OUTPUT);
  setupTimerPWM();

  // sw
  pinMode(FAN_SW, OUTPUT);
  digitalWrite(FAN_SW, LOW);
}

void setupWatchdog() {
  wdt_enable(WDTO_4S);
}

void setupTempSensor() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  resetTempSensor();
  for (float& temp : temp_data) {
    temp = TEMP_LIMIT;
  }
  temp_sum = TEMP_COUNT * TEMP_LIMIT;
}

void requestTemperature() {
  digitalWrite(LED_BUILTIN, HIGH);
  temp_sensor.requestTemperatures();
}

void tempReadError(const int times = 3) {
  for (int i = 0; i < times; ++i) {
    digitalWrite(LED_BUILTIN, LOW);
    delay(BLINK_DELAY);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(BLINK_DELAY);
  }
  digitalWrite(LED_BUILTIN, LOW);
}

float readTemperature() {
  if (IS_TEMP_READING && temp_sensor.isConversionComplete()) {
    float temp_read = temp_sensor.getTempCByIndex(0);
    if (temp_read != DEVICE_DISCONNECTED_C && temp_read <= TEMP_LIMIT) {
      temp_read *= TEMP_CALIBRATION;
      temp_sum -= temp_data[temp_index];
      temp_data[temp_index] = temp_read;
      temp_sum += temp_read;
      temp_index = (temp_index + 1) % TEMP_COUNT;
      digitalWrite(LED_BUILTIN, LOW);
      return TEMP_AVG;
    }
  }
  return DEVICE_DISCONNECTED_C;
}

void setFan(const float temp_read) {
  const bool fan_status = FAN_READ ? (temp_read >= TEMP_OFF) : (temp_read >= TEMP_ON);
  digitalWrite(FAN_SW, fan_status);

  if (fan_status) {
    const float fan = (temp_read - TEMP_OFF) / (TEMP_LIMIT - TEMP_OFF);
    fan_pwm = constrain(fan, PWM_FAN_MIN, PWM_FAN_MAX);
    SET_FAN_PWM(fan_pwm);
  }
}

void setup() {
  setupSerial();
  setupWatchdog();
  setupTempSensor();
  setupFanControl();

  setFan(TEMP_AVG);
}

void loop() {
  // read temperature and set fan status and speed
  const float temp_read = readTemperature();
  if (temp_read != DEVICE_DISCONNECTED_C) {
    setFan(temp_read);
  }

  if (timer.isReady()) {
    if (IS_TEMP_READING) {
      tempReadError();
      resetTempSensor();
      printTempReadError(temp_read);
    } else {
      printStatus();
    }
    requestTemperature();
    timer.reset();
  }

  wdt_reset();

  delay(20);
}
