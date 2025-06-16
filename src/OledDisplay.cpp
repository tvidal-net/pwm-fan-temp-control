#ifdef ESP8266
#include "OledDisplay.h"

#define OLED_WIDTH       0x80
#define OLED_HEIGHT      0x40
#define OLED_RESET        (-1)

#define OLED_TEXT_SIZE   0x02
#define OLED_ROW_SIZE    0x08

#define OLED_FAN_ROW     0x00
#define OLED_TEMP_ROW    0x01

#define OLED_I2C_ADDRESS 0x3C

#define BUFFER_SIZE      0x80

constexpr int16_t row_height = OLED_ROW_SIZE * OLED_TEXT_SIZE;

void OledDisplay::clearRow(const int16_t row) {
  const int16_t y = row * row_height;
  m_Display.fillRect(0, y, OLED_WIDTH, row_height, BLACK);
  m_Display.setCursor(0, y);
}

void OledDisplay::printValue(const float value, const char label, const char unit) {
  m_Display.write(label);
  m_Display.print(F(" ="));
  if (value < 100.0f) {
    m_Display.write(' ');
  }
  if (value < 10.0f) {
    m_Display.write(' ');
  }
  if (value > -100.0f) {
    m_Display.write(' ');
    m_Display.print(value, 1);
    m_Display.write(' ');
    m_Display.write(unit);
  } else {
    m_Display.print(value, 0);
  }
}

void OledDisplay::drawChart() {
  constexpr int16_t chart_height = 2 * row_height;
  m_Display.fillRect(0, chart_height, OLED_WIDTH, chart_height, BLACK);
  for (int16_t x = 0; x < OLED_WIDTH; ++x) {
    const uint8_t i = (x + m_ChartIndex) % OLED_WIDTH;
    const int16_t y = m_Chart[i];
    m_Display.drawPixel(x, y, WHITE);
  }
}

void OledDisplay::setChart(const float temp_c) {
  m_Chart[m_ChartIndex] = OLED_HEIGHT + 0x10 - temp_c;
  m_ChartIndex = (m_ChartIndex + 1) % OLED_WIDTH;
  this->drawChart();
}

void OledDisplay::resetChart() const {
  for (uint8_t i = 0; i < OLED_WIDTH; ++i) {
    m_Chart[i] = OLED_HEIGHT;
  }
}

OledDisplay::OledDisplay(const uint8_t sda, const uint8_t scl) :
  m_Display(OLED_WIDTH, OLED_HEIGHT, &m_TwoWire, OLED_RESET) {
  m_TwoWire.begin(sda, scl);
  m_Chart = new uint8_t[OLED_WIDTH];
  this->resetChart();
}

OledDisplay::~OledDisplay() {
  delete[] m_Chart;
}

bool OledDisplay::begin() {
  if (m_Display.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDRESS)) {
    m_Display.clearDisplay();
    m_Display.setTextColor(WHITE);
    m_Display.setTextSize(OLED_TEXT_SIZE);
    m_Display.setTextWrap(false);
    m_Display.cp437(true);
    m_Display.display();
    return true;
  }
  return false;
}

void OledDisplay::clear() {
  m_Display.clearDisplay();
}

void OledDisplay::show() {
  m_Display.display();
}

void OledDisplay::setTempC(const float temp_c) {
  clearRow(OLED_TEMP_ROW);
  setChart(temp_c);
  printValue(temp_c, 'T', 'C');
}

void OledDisplay::setFanPWM(const float fan_pwm) {
  clearRow(OLED_FAN_ROW);
  printValue(100 * fan_pwm, 'F', '%');
}
#endif
