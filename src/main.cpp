#include <Arduino.h>
#include <avr/wdt.h>
#include <OneWire.h>
#include <OneButton.h>
#include <SimpleTimer.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS        PIN2
#define FAN_PWM             PIN3
#define FAN_SW              PIN4
#define BTN_SW              PIN7

#define FAN_PWM_HIGH        79

#define SENSOR_RESOLUTION   12
#define TIMER_INTERVAL      2000
#define BLINK_DELAY         50

#define PWM_FAN_MIN         0.2f
#define PWM_FAN_MAX         1.0f

#define TEMP_COUNT          8
#define TEMP_MIN            28.0f
#define TEMP_MAX            36.0f
#define TEMP_LIMIT          55.0f

#define IS_TEMP_READING     digitalRead(LED_BUILTIN)
#define TEMP_AVG            (temp_sum / TEMP_COUNT)
#define FAN_READ            digitalRead(FAN_SW)

#define SET_FAN_PWM(pwm)    OCR2B = (uint8_t)(FAN_PWM_HIGH*(pwm+0.5f))

const char* ON = "ON";
const char* OFF = "OFF";

OneWire one_wire_bus(ONE_WIRE_BUS);
DallasTemperature temp_sensor(&one_wire_bus);

SimpleTimer timer(TIMER_INTERVAL);

OneButton button(BTN_SW);

uint8_t temp_index = 0;
float temp_sum = 0.0f;
float temp_data[TEMP_COUNT];

float fan_pwm = 0.0f;

bool button_override = false;

static
void buttonPress() {
  button_override = !button_override;
}

void setupSerial() {
#ifdef DEBUG
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

  temp_sensor.begin();
  temp_sensor.setWaitForConversion(false);
  temp_sensor.setResolution(SENSOR_RESOLUTION);

  for (float& temp : temp_data) {
    temp = 0.0f;
  }
  temp_sum = 0.0f;
}

void setupButton() {
  button.attachLongPressStart(buttonPress);
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
    const float temp_read = temp_sensor.getTempCByIndex(0);
    if (temp_read != DEVICE_DISCONNECTED_C && temp_read <= TEMP_LIMIT) {
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
  const bool fan_status = FAN_READ ? temp_read >= TEMP_MIN : temp_read >= TEMP_MAX;
  digitalWrite(FAN_SW, button_override || fan_status);

  if (FAN_READ) {
    fan_pwm = constrain((temp_read - TEMP_MIN) / (TEMP_MAX - TEMP_MIN), PWM_FAN_MIN, PWM_FAN_MAX);
    SET_FAN_PWM(fan_pwm);
  }
}

void printTempData(const float temp_read) {
  Serial.print("temp[");
  Serial.print(temp_data[0], 1);
  for (const float& temp : temp_data) {
    Serial.print(",");
    Serial.print(temp, 1);
  }
  Serial.print("] sum=");
  Serial.print(temp_sum, 1);
  Serial.print("/");
  Serial.print(TEMP_COUNT);
  Serial.print("=");
  Serial.print(temp_read);
  Serial.print("°C");
}

void printFanStatus(const bool fan) {
  Serial.print(" fan=");
  Serial.print(fan ? ON : OFF);
  if (fan) {
    Serial.print(" pwm=");
    Serial.print(fan_pwm, 2);
  }
}

void printButtonStatus(const bool button) {
  Serial.print(" btn=");
  Serial.print(button ? ON : OFF);
}

void printStatus() {
#ifdef DEBUG
  printTempData(temp_read);
  printFanStatus(fan_read);
  // printButtonStatus(button_override);
  Serial.println();
#endif
}

void setup() {
  setupSerial();
  setupButton();
  setupWatchdog();
  setupTempSensor();
  setupFanControl();

  printStatus();
}

void loop() {
  // read temperature and set fan status and speed
  const float temp_read = readTemperature();
  if (temp_read != DEVICE_DISCONNECTED_C) {
    setFan(temp_read);
  }

  if (timer.isReady()) {
    printStatus();
    if (IS_TEMP_READING) {
      tempReadError();
      setupTempSensor();
    }
    requestTemperature();
    timer.reset();
  }

  wdt_reset();
  button.tick();

  delay(20);
}
