#ifndef HVAC_WEEK3_DISPLAY_UI_H
#define HVAC_WEEK3_DISPLAY_UI_H

#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <XPT2046_Touchscreen.h>
#include "SensorData.h"
#include "config.h"

class DisplayUI {
public:
  DisplayUI();
  void begin();
  void update(const SensorData& data, const SystemState& state);
  bool touched();

private:
  Adafruit_ILI9341 _tft;
  XPT2046_Touchscreen _touch;
  uint16_t colorForLevel(SystemLevel level);
  void drawHeader(const SystemState& state);
  void drawSensorLine(int y, const char* label, const String& value, bool ok);
};

#endif
