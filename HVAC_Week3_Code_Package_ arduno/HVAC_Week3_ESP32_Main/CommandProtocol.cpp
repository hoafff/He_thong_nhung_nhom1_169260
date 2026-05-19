#include "CommandProtocol.h"
#include "RuleEngine.h"

static String boolToCommand(bool on) {
  return on ? "ON" : "OFF";
}

String makeCommandJson(
  const char* target,
  bool turnOn,
  const char* reason,
  const char* sourceSensor,
  float value,
  const char* unit,
  const SystemState& state,
  uint32_t sequenceNo
) {
  String json = "{";
  json += "\"seq\":" + String(sequenceNo) + ",";
  json += "\"target\":\"" + String(target) + "\",";
  json += "\"command\":\"" + boolToCommand(turnOn) + "\",";
  json += "\"mode\":\"AUTO\",";
  json += "\"reason\":\"" + String(reason) + "\",";
  json += "\"source_sensor\":\"" + String(sourceSensor) + "\",";
  json += "\"value\":" + String(value, 2) + ",";
  json += "\"unit\":\"" + String(unit) + "\",";
  json += "\"system_status\":\"" + levelToString(state.level) + "\",";
  json += "\"timestamp_ms\":" + String(millis());
  json += "}";
  return json;
}

String makeSensorJson(const SensorData& data, const SystemState& state, uint32_t sequenceNo) {
  String json = "{";
  json += "\"seq\":" + String(sequenceNo) + ",";
  json += "\"temperature_c\":" + String(data.temperatureC, 2) + ",";
  json += "\"humidity_percent\":" + String(data.humidityPercent, 2) + ",";
  json += "\"pm1_0\":" + String(data.pm1_0) + ",";
  json += "\"pm2_5\":" + String(data.pm2_5) + ",";
  json += "\"pm10\":" + String(data.pm10) + ",";
  json += "\"mq135_raw\":" + String(data.mq135Raw) + ",";
  json += "\"mq135_voltage\":" + String(data.mq135Voltage, 3) + ",";
  json += "\"status\":\"" + levelToString(state.level) + "\",";
  json += "\"reason\":\"" + state.reason + "\",";
  json += "\"fan_on\":" + String(state.fanOn ? "true" : "false") + ",";
  json += "\"hvac_on\":" + String(state.hvacOn ? "true" : "false") + ",";
  json += "\"timestamp_ms\":" + String(data.timestampMs);
  json += "}";
  return json;
}
