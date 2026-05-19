#ifndef HVAC_WEEK3_MQ135_DRIVER_H
#define HVAC_WEEK3_MQ135_DRIVER_H

#include <Arduino.h>
#include "SensorData.h"

class MQ135Driver {
public:
  MQ135Driver(int analogPin, int avgSamples);
  void begin();
  bool read(SensorData& data);

private:
  int _pin;
  int _samples;
};

#endif
