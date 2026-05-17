#ifndef HVAC_WEEK3_DHT22_DRIVER_H
#define HVAC_WEEK3_DHT22_DRIVER_H

#include <Arduino.h>
#include <DHT.h>
#include "SensorData.h"

class DHT22Driver {
public:
  explicit DHT22Driver(int dataPin);
  void begin();
  bool read(SensorData& data);

private:
  DHT _dht;
};

#endif
