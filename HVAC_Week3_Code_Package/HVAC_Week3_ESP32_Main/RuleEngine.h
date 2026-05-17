#ifndef HVAC_WEEK3_RULE_ENGINE_H
#define HVAC_WEEK3_RULE_ENGINE_H

#include <Arduino.h>
#include "SensorData.h"

SystemState evaluateRules(const SensorData& data);
String levelToString(SystemLevel level);

#endif
