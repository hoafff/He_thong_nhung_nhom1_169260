#ifndef HVAC_WEEK3_SENSOR_DATA_H
#define HVAC_WEEK3_SENSOR_DATA_H

#include <Arduino.h>

struct SensorData {
  uint32_t timestampMs = 0;

  // DHT22
  float temperatureC = NAN;
  float humidityPercent = NAN;
  bool dhtOk = false;

  // PMS5003 atmospheric values
  uint16_t pm1_0 = 0;
  uint16_t pm2_5 = 0;
  uint16_t pm10 = 0;
  bool pmsOk = false;

  // MQ-135 analog indicator
  int mq135Raw = 0;
  float mq135Voltage = 0.0f;
  bool mq135Ok = false;
};

enum class SystemLevel {
  NORMAL,
  WARNING,
  DANGER,
  SENSOR_ERROR
};

struct SystemState {
  SystemLevel level = SystemLevel::SENSOR_ERROR;
  String reason = "INIT";
  bool fanOn = false;
  bool hvacOn = false;

  bool equals(const SystemState& other) const {
    return level == other.level && reason == other.reason && fanOn == other.fanOn && hvacOn == other.hvacOn;
  }
};

#endif
