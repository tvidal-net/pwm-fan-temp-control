// ***************************************************
// ** pwm-fan-temp-control/src/main.cpp
// ***************************************************
// vim: ts=2 sw=2 et:

#include <Arduino.h>
#ifdef ESP8266
#include "OledDisplay.h"
#else
#include <avr/wdt.h>
#endif

#include "Fan.h"
#include "Led.h"
#include "Sensor.h"

#define DELAY             0x800

#define TEMP_OFF          30.0f
#define TEMP_ON           40.0f
#define TEMP_MAX          60.0f

#ifdef ESP8266
#define TEMP_SENSOR_PIN      D2
#define FAN_PWM_PIN          D3
#define FAN_SW_PIN           D6
#define SERIAL_BAUD      115200
#else
#define TEMP_SENSOR_PIN    PIN2
#define FAN_SW_PIN         PIN3
#define FAN_PWM_PIN        PIN4
#define SERIAL_BAUD        9600
#endif

static
const Led led;

static
Sensor sensor(TEMP_SENSOR_PIN, &led);

static
Fan fan(FAN_SW_PIN, FAN_PWM_PIN);

#ifdef ESP8266
static
OledDisplay display;
#endif

#ifndef RELEASE
static
void print_error() {
  Serial.print("- ERROR: ");
}
#endif

static
void print_temp_c(const float temp_c) {
#ifndef RELEASE
  Serial.print(F("temp = "));
  Serial.print(temp_c, 1);
  Serial.print(F(" °C"));
#endif
}

static
void print_fan_pwm(const float fan_pwm) {
#ifndef RELEASE
  Serial.print(F(", fan = "));
  Serial.print(100.0f * fan_pwm, 1);
  Serial.print(F(" %"));
#endif
}

static
void print_status() {
#ifndef RELEASE
  Serial.print(F("- "));
#endif
}

static
void println() {
#ifndef RELEASE
  Serial.println();
#endif
}

static
void display_temp_c(const float temp_c) {
#ifdef ESP8266
  display.setTempC(temp_c);
  display.show();
#endif
}

static
void display_fan_pwm(const float fan_pwm) {
#ifdef ESP8266
  display.setFanPWM(fan_pwm);
  display.show();
#endif
}

static
void display_clear() {
#ifdef ESP8266
  display.clear();
  display.show();
#endif
}

#ifdef ESP8266
static
void setup_display_error() {
#ifndef RELEASE
  print_error();
  Serial.println(F("Failed to initialize display"));
#endif
}
#endif

static
void temp_read_error(const float temp_c) {
#ifndef RELEASE
  print_error();
  Serial.print(F("read = "));
  Serial.println(temp_c, 0);
#endif
#ifdef ESP8266
  display_temp_c(temp_c);
#endif
  led.blink();
}

static
void setup_serial() {
#ifndef RELEASE
  Serial.begin(SERIAL_BAUD);
  Serial.println("\n\npwm-fan-temp-control");
#endif
}

static
void setup_display() {
#ifdef ESP8266
  if (!display.begin()) {
    setup_display_error();
    for (;;) {
      led.blink();
    }
  }
#endif
}

static
void off() {
  fan.off();
  display_clear();
}

static
void set_fan_pwm(const float temp_c) {
  const float fan_pwm = (temp_c - TEMP_OFF) / (TEMP_MAX - TEMP_OFF);
  fan.write(fan_pwm);

  display_fan_pwm(fan_pwm);
  print_fan_pwm(fan_pwm);
}

void setup() {
  setup_serial();
  setup_display();
}

void loop() {
  const float temp_c = sensor.getTempC();
  if (Sensor::isValid(temp_c) && temp_c < TEMP_MAX) {
    print_status();
    print_temp_c(temp_c);
    if (temp_c < TEMP_OFF) {
      off();
    } else if (temp_c > TEMP_ON || fan.read()) {
      display_temp_c(temp_c);
      set_fan_pwm(temp_c);
    }
    println();
  } else {
    temp_read_error(temp_c);
  }
  delay(DELAY);
}
