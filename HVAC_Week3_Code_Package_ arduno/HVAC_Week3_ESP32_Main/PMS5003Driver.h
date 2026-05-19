#ifndef HVAC_WEEK3_PMS5003_DRIVER_H
#define HVAC_WEEK3_PMS5003_DRIVER_H

#include <Arduino.h>
#include "SensorData.h"

class PMS5003Driver {
public:
  explicit PMS5003Driver(HardwareSerial& serialPort);
  void begin(uint32_t baud, int rxPin, int txPin);
  bool read(SensorData& data);

private:
  HardwareSerial& _serial;
  static uint16_t u16(const uint8_t highByte, const uint8_t lowByte);
  bool readFrame(uint8_t frame[32]);
};

#endif
