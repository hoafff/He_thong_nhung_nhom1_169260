#ifndef HVAC_WEEK3_DHT22_DRIVER_H
#define HVAC_WEEK3_DHT22_DRIVER_H

#include <Arduino.h>
#include "SensorData.h"

class DHT22Driver {
public:
  explicit DHT22Driver(int dataPin);
  void begin();
  bool read(SensorData& data);

private:
  int _pin;
  bool waitForLevel(uint8_t level, uint32_t timeoutUs);
};

#endif
