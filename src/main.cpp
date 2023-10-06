#include <Arduino.h>
#include <avr/wdt.h>
#include <OneWire.h>
#include <OneButton.h>
#include <SimpleTimer.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS        PIN2
#define FAN_PWM             PIN3
#define BTN_SW              PIN4
#define FAN_SW              PIN5

#define FAN_PWM_HIGH        79

#define SENSOR_RESOLUTION   9
#define TIMER_INTERVAL      666
#define BLINK_DELAY         66

#define TEMP_MIN            26.0f
#define TEMP_MAX            32.0f
#define TEMP_LIMIT          50.0f

#define isTempReading       digitalRead(LED_BUILTIN)
#define temp_read           (temp_sum / temp_count)
#define fan_read            digitalRead(FAN_SW)

#define setFanPWM(pwm)      OCR2B = (uint8_t)(FAN_PWM_HIGH*(pwm+0.5f))

const char *ON = "ON";
const char *OFF = "OFF";

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

SimpleTimer timer(TIMER_INTERVAL);

OneButton button(BTN_SW);

const uint8_t temp_count = 6;
uint8_t temp_index = 0;
float temp_sum = 0.0f;
float temp_data[temp_count];

float fan_pwm = 0.0f;

bool button_override = false;

static void buttonPress() {
  button_override = !button_override;
}

void setupSerial() {
#ifdef DEBUG
  Serial.begin(9600);
  Serial.print("\n\npstemp-control ");
#endif
}

void setupTimer() {
  TIMSK2 = TIFR2 = 0;
  TCCR2A = (1 << COM2B1) | (1 << WGM21) | (1 << WGM20);
  TCCR2B = (1 << WGM22) | (1 << CS21);
  OCR2A = FAN_PWM_HIGH;
  OCR2B = 0;
}

void setupFanControl() {
  // pwm
  pinMode(FAN_PWM, OUTPUT);
  setupTimer();

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

  sensors.begin();
  sensors.setWaitForConversion(false);
  sensors.setResolution(SENSOR_RESOLUTION);

  for (int i = 0; i < temp_count; ++i) {
    temp_data[i] = 0.0f;
  }
  temp_sum = 0.0f;
}

void setupButton() {
  button.attachLongPressStart(buttonPress);
}

void requestTemperature() {
  digitalWrite(LED_BUILTIN, HIGH);
  sensors.requestTemperatures();
}

void tempReadError(int times = 3) {
  for (int i = 0; i < times; ++i) {
    digitalWrite(LED_BUILTIN, LOW);
    delay(BLINK_DELAY);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(BLINK_DELAY);
  }
  digitalWrite(LED_BUILTIN, LOW);
}

float readTemperature() {
  if (isTempReading && sensors.isConversionComplete()) {
    digitalWrite(LED_BUILTIN, LOW);
    const float temp = sensors.getTempCByIndex(0);
    if (temp != DEVICE_DISCONNECTED_C && temp <= TEMP_LIMIT) {
      temp_sum -= temp_data[temp_index];
      temp_data[temp_index] = temp;
      temp_sum += temp;
      temp_index = (temp_index + 1) % temp_count;
      return temp_read;
    }
  }
  return DEVICE_DISCONNECTED_C;
}

void setFan(const float temp) {
  const bool fan_status = fan_read ? (temp >= TEMP_MIN) : (temp >= TEMP_MAX);
  digitalWrite(FAN_SW, button_override || fan_status);

  if (fan_read) {
    fan_pwm = constrain((temp - TEMP_MIN) / (TEMP_MAX - TEMP_MIN), 0.2f, 1.0f);
    setFanPWM(fan_pwm);
  }
}

void printTemperatureStatus(const float temp) {
  Serial.print("temp[");
  Serial.print(temp_data[0], 1);
  for (int i = 1; i < temp_count; ++i) {
    Serial.print(", ");
    Serial.print(temp_data[i], 1);
  }
  Serial.print("] sum=");
  Serial.print(temp_sum, 1);
  Serial.print("/");
  Serial.print(temp_count);
  Serial.print("=");
  Serial.print(temp);
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
  printTemperatureStatus(temp_read);
  printFanStatus(fan_read);
  printButtonStatus(button_override);
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
  wdt_reset();
  button.tick();

  if (timer.isReady()) {
    printStatus();
    if (isTempReading) {
      tempReadError();
      setupTempSensor();
    }
    requestTemperature();
    timer.reset();
  }

  // read temperature and set fan status and speed
  const float temperature = readTemperature();
  if (temperature != DEVICE_DISCONNECTED_C) {
    setFan(temperature);
  }
  delay(1);
}
