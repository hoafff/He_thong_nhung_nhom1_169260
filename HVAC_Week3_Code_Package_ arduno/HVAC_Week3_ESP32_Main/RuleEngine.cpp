#include "RuleEngine.h"
#include "config.h"

static void raiseLevel(SystemState& state, SystemLevel newLevel, const String& reason) {
  if (static_cast<int>(newLevel) > static_cast<int>(state.level)) {
    state.level = newLevel;
    state.reason = reason;
  }
}

SystemState evaluateRules(const SensorData& data) {
  SystemState state;
  state.level = SystemLevel::NORMAL;
  state.reason = "ALL_NORMAL";
  state.fanOn = false;
  state.hvacOn = false;

  if (!data.dhtOk && !data.pmsOk && !data.mq135Ok) {
    state.level = SystemLevel::SENSOR_ERROR;
    state.reason = "NO_SENSOR_DATA";
    return state;
  }

  // PM2.5 -> ventilation fan
  if (data.pmsOk) {
    if (data.pm2_5 > PM25_DANGER_UGM3) {
      state.fanOn = true;
      raiseLevel(state, SystemLevel::DANGER, "PM25_DANGER");
    } else if (data.pm2_5 > PM25_WARNING_UGM3) {
      state.fanOn = true;
      raiseLevel(state, SystemLevel::WARNING, "PM25_WARNING");
    }
  }

  // MQ-135 raw air-quality indicator -> ventilation fan
  if (data.mq135Ok) {
    if (data.mq135Raw > MQ135_DANGER_RAW) {
      state.fanOn = true;
      raiseLevel(state, SystemLevel::DANGER, "AIR_QUALITY_DANGER");
    } else if (data.mq135Raw > MQ135_WARNING_RAW) {
      state.fanOn = true;
      raiseLevel(state, SystemLevel::WARNING, "AIR_QUALITY_WARNING");
    }
  }

  // Temperature -> HVAC
  if (data.dhtOk) {
    if (data.temperatureC > TEMP_DANGER_C) {
      state.hvacOn = true;
      raiseLevel(state, SystemLevel::DANGER, "TEMP_DANGER");
    } else if (data.temperatureC > TEMP_WARNING_C) {
      state.hvacOn = true;
      raiseLevel(state, SystemLevel::WARNING, "TEMP_WARNING");
    }

    // Humidity -> ventilation fan
    if (data.humidityPercent > HUMID_DANGER_PERCENT) {
      state.fanOn = true;
      raiseLevel(state, SystemLevel::DANGER, "HUMIDITY_DANGER");
    } else if (data.humidityPercent > HUMID_WARNING_PERCENT) {
      state.fanOn = true;
      raiseLevel(state, SystemLevel::WARNING, "HUMIDITY_WARNING");
    }
  }

  return state;
}

String levelToString(SystemLevel level) {
  switch (level) {
    case SystemLevel::NORMAL: return "NORMAL";
    case SystemLevel::WARNING: return "WARNING";
    case SystemLevel::DANGER: return "DANGER";
    case SystemLevel::SENSOR_ERROR: return "SENSOR_ERROR";
  }
  return "UNKNOWN";
}
