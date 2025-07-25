// ***************************************************
// ** pwm-fan-temp-control/src/main.cpp
// ***************************************************
// vim: ts=2 sw=2 et:

#include <Arduino.h>
#include <string>

#include "Network.h"

#include "OledDisplay.h"

#include "Fan.h"
#include "Led.h"
#include "Sensor.h"

#ifndef SERIAL_BAUD
#define SERIAL_BAUD             9600
#endif

#define DELAY                  0x800

#define TEMP_OFF               30.0f
#define TEMP_ON                40.0f
#define TEMP_MAX               60.0f

#define OFF_TIME     60u * 60 * 1000

#define TEMP_SENSOR_PIN           D2
#define FAN_PWM_PIN               D4
#define FAN_SW_PIN                D7

static
const Led led;

static
Sensor sensor(TEMP_SENSOR_PIN, &led);

static
Fan fan(FAN_SW_PIN, FAN_PWM_PIN);

static
OledDisplay display;

static
Network network;

static
void print_error() {
  Serial.print("- ERROR: ");
}

static
void print_temp_c(const float temp_c) {
  Serial.print(F("temp = "));
  Serial.print(temp_c, 1);
  Serial.print(F(" °C"));
}

static
void print_fan_pwm(const float fan_pwm) {
  Serial.print(F(", fan = "));
  Serial.print(100.0f * fan_pwm, 1);
  Serial.print(F(" %"));
}

static
void print_status() {
  Serial.print(F("- "));
}

static
void println() {
  Serial.println();
}

static
void display_temp_c(const float temp_c) {
  display.setTempC(temp_c);
  display.show();
}

static
void display_fan_pwm(const float fan_pwm) {
  display.setFanPWM(fan_pwm);
  display.show();
}

static
void display_clear() {
  display.clear();
  display.show();
}

static
void setup_display_error() {
  print_error();
  Serial.println(F("Failed to initialize display"));
}

static
void temp_read_error(const float temp_c) {
  print_error();
  Serial.print(F("read = "));
  Serial.println(temp_c, 0);
  display_temp_c(temp_c);
  led.blink();
}

static
void setup_serial() {
  Serial.begin(SERIAL_BAUD);
  Serial.println("\n\npwm-fan-temp-control");
}

static
void setup_display() {
  if (!display.begin()) {
    setup_display_error();
    for (;;) {
      led.blink();
    }
  }
}

static
void setup_network() {
  network.startServer();
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
  network.send(temp_c, fan_pwm);
}

void setup() {
  setup_serial();
  setup_display();
  setup_network();
}

void loop() {
  static uint32_t off_time = 0;
  const auto temp_c = sensor.getTempC();
  if (Sensor::isValid(temp_c) && temp_c < TEMP_MAX) {
    print_status();
    print_temp_c(temp_c);
    if (temp_c > TEMP_ON || fan.read()) {
      set_fan_pwm(temp_c);
      display_temp_c(temp_c);
    }
    if (temp_c < TEMP_OFF) {
      if (off_time) {
        const auto now = millis();
        const auto elapsed = now - off_time;
        if (elapsed > OFF_TIME) {
          off_time = 0;
          off();
        }
      } else if (fan.read()) {
        off_time = millis();
      }
    }
    println();
  } else {
    temp_read_error(temp_c);
    network.send(temp_c);
  }
  const auto cmd = network.pop();
  if (!cmd.empty()) {
    Serial.print("received command = ");
    Serial.println(cmd.c_str());
    const char ch = cmd[0];
    const uint8_t value = cmd[1] - '0';
    if (ch == 'F') {
      if (value > 0) {
        fan.write(10.0f * value);
        display_fan_pwm(value / 10.0f);
      } else {
        off();
      }
    }
  }
  delay(DELAY);
}
