#ifndef OLEDDISPLAY_H
#define OLEDDISPLAY_H

#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <Wire.h>

#define OLED_SDA_PIN D5
#define OLED_SCL_PIN D6

class OledDisplay final {
  uint8_t m_ChartIndex = 0;
  uint8_t* m_Chart;
  TwoWire m_TwoWire;
  Adafruit_SSD1306 m_Display;

  void clearRow(int16_t row);

  void printValue(float value, char label, char unit);

  void drawChart();

  void setChart(float temp_c);

  void resetChart() const;

public:
  explicit OledDisplay(uint8_t sda = OLED_SDA_PIN, uint8_t scl = OLED_SCL_PIN);

  ~OledDisplay();

  bool begin();

  void clear();

  void show();

  void setTempC(float temp_c);

  void setFanPWM(float fan_pwm);
};

#endif //OLEDDISPLAY_H
