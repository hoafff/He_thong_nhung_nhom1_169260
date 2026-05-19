#include "DisplayUI.h"
#include "RuleEngine.h"

DisplayUI::DisplayUI()
  : _tft(TFT_CS, TFT_DC, TFT_RST), _touch(TOUCH_CS, TOUCH_IRQ) {}

void DisplayUI::begin() {
  SPI.begin(TFT_SCLK, TFT_MISO, TFT_MOSI);
  _tft.begin();
  _tft.setRotation(1);  // landscape 320x240
  _tft.fillScreen(ILI9341_BLACK);
  _tft.setTextWrap(false);

  _touch.begin();
  _touch.setRotation(1);

  _tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  _tft.setTextSize(2);
  _tft.setCursor(18, 100);
  _tft.print("HVAC Week 3 Booting...");
  delay(800);
}

bool DisplayUI::touched() {
  return _touch.touched();
}

uint16_t DisplayUI::colorForLevel(SystemLevel level) {
  switch (level) {
    case SystemLevel::NORMAL: return ILI9341_GREEN;
    case SystemLevel::WARNING: return ILI9341_YELLOW;
    case SystemLevel::DANGER: return ILI9341_RED;
    case SystemLevel::SENSOR_ERROR: return ILI9341_ORANGE;
  }
  return ILI9341_WHITE;
}

void DisplayUI::drawHeader(const SystemState& state) {
  _tft.fillRect(0, 0, 320, 42, colorForLevel(state.level));
  _tft.setTextColor(ILI9341_BLACK);
  _tft.setTextSize(2);
  _tft.setCursor(8, 7);
  _tft.print("Smart HVAC - W3");
  _tft.setCursor(205, 7);
  _tft.print(levelToString(state.level));
}

void DisplayUI::drawSensorLine(int y, const char* label, const String& value, bool ok) {
  _tft.setTextSize(2);
  _tft.setTextColor(ILI9341_CYAN, ILI9341_BLACK);
  _tft.setCursor(12, y);
  _tft.print(label);

  _tft.fillRect(140, y - 2, 170, 20, ILI9341_BLACK);
  _tft.setTextColor(ok ? ILI9341_WHITE : ILI9341_RED, ILI9341_BLACK);
  _tft.setCursor(140, y);
  _tft.print(ok ? value : "NO DATA");
}

void DisplayUI::update(const SensorData& data, const SystemState& state) {
  _tft.fillScreen(ILI9341_BLACK);
  drawHeader(state);

  drawSensorLine(55, "Temp", String(data.temperatureC, 1) + " C", data.dhtOk);
  drawSensorLine(82, "Humidity", String(data.humidityPercent, 1) + " %", data.dhtOk);
  drawSensorLine(109, "PM2.5", String(data.pm2_5) + " ug/m3", data.pmsOk);
  drawSensorLine(136, "PM10", String(data.pm10) + " ug/m3", data.pmsOk);
  drawSensorLine(163, "MQ135", String(data.mq135Raw) + " / " + String(data.mq135Voltage, 2) + "V", data.mq135Ok);

  _tft.drawFastHLine(0, 194, 320, ILI9341_DARKGREY);
  _tft.setTextSize(2);
  _tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  _tft.setCursor(10, 204);
  _tft.print("Fan:");
  _tft.setTextColor(state.fanOn ? ILI9341_GREEN : ILI9341_LIGHTGREY, ILI9341_BLACK);
  _tft.print(state.fanOn ? " ON " : " OFF");

  _tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  _tft.setCursor(155, 204);
  _tft.print("HVAC:");
  _tft.setTextColor(state.hvacOn ? ILI9341_GREEN : ILI9341_LIGHTGREY, ILI9341_BLACK);
  _tft.print(state.hvacOn ? "ON" : "OFF");

  _tft.setTextSize(1);
  _tft.setTextColor(ILI9341_LIGHTGREY, ILI9341_BLACK);
  _tft.setCursor(10, 230);
  _tft.print("Reason: ");
  _tft.print(state.reason);
}
