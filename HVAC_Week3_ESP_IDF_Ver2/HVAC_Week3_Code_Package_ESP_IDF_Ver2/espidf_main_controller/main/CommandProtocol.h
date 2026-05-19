#ifndef HVAC_WEEK3_COMMAND_PROTOCOL_H
#define HVAC_WEEK3_COMMAND_PROTOCOL_H

#include <Arduino.h>
#include "SensorData.h"

String makeCommandJson(
  const char* target,
  bool turnOn,
  const char* reason,
  const char* sourceSensor,
  float value,
  const char* unit,
  const SystemState& state,
  uint32_t sequenceNo
);

String makeSensorJson(const SensorData& data, const SystemState& state, uint32_t sequenceNo);

#endif
