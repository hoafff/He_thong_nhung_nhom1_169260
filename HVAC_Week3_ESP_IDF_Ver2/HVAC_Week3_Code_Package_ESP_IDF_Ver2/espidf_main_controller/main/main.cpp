#include <Arduino.h>
#include "config.h"
#include "SensorData.h"
#include "PMS5003Driver.h"
#include "MQ135Driver.h"
#include "DHT22Driver.h"
#include "RuleEngine.h"
#include "DisplayUI.h"
#include "CommandProtocol.h"

#if ENABLE_WIFI_AP || ENABLE_WIFI_STA || ENABLE_LOCAL_HTTP_COMMANDS || ENABLE_CLOUD_UPLOAD
  #include <WiFi.h>
#endif
#if ENABLE_LOCAL_HTTP_COMMANDS || ENABLE_CLOUD_UPLOAD
  #include <HTTPClient.h>
#endif

PMS5003Driver pms(Serial2);
MQ135Driver mq135(MQ135_AO_PIN, MQ135_AVG_SAMPLES);
DHT22Driver dht22(DHT22_PIN);
DisplayUI display;

SensorData sensorData;
SystemState currentState;
SystemState lastSentState;
bool firstCommandSend = true;

uint32_t sequenceNo = 0;
uint32_t lastSensorReadMs = 0;
uint32_t lastDisplayRefreshMs = 0;
uint32_t lastCommandSentMs = 0;

void setupWifi() {
#if ENABLE_WIFI_AP || ENABLE_WIFI_STA
  WiFi.mode(WIFI_OFF);
  delay(100);
#endif

#if ENABLE_WIFI_AP
  WiFi.mode(WIFI_AP);
  bool ok = WiFi.softAP(AP_SSID, AP_PASSWORD);
  Serial.println(ok ? "[WiFi] SoftAP started" : "[WiFi] SoftAP failed");
  Serial.print("[WiFi] AP SSID: "); Serial.println(AP_SSID);
  Serial.print("[WiFi] AP IP: "); Serial.println(WiFi.softAPIP());
#endif

#if ENABLE_WIFI_STA
  WiFi.mode(WIFI_STA);
  WiFi.begin(STA_SSID, STA_PASSWORD);
  Serial.print("[WiFi] Connecting STA");
  uint32_t start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 15000) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("[WiFi] STA IP: "); Serial.println(WiFi.localIP());
  } else {
    Serial.println("[WiFi] STA connection timeout");
  }
#endif
}

void sendHttpJson(const char* url, const String& jsonBody) {
#if ENABLE_LOCAL_HTTP_COMMANDS || ENABLE_CLOUD_UPLOAD
  HTTPClient http;
  http.setTimeout(3000);
  if (!http.begin(url)) {
    Serial.print("[HTTP] begin failed: "); Serial.println(url);
    return;
  }

  http.addHeader("Content-Type", "application/json");
  int code = http.POST(jsonBody);
  Serial.print("[HTTP] POST "); Serial.print(url); Serial.print(" -> "); Serial.println(code);
  if (code > 0) {
    Serial.print("[HTTP] Response: "); Serial.println(http.getString());
  }
  http.end();
#else
  (void)url;
  (void)jsonBody;
#endif
}

void printSensorLog(const SensorData& data, const SystemState& state) {
  Serial.println("---------------- WEEK 3 SENSOR LOG ----------------");
  Serial.print("Temp: "); Serial.print(data.temperatureC, 1); Serial.println(" C");
  Serial.print("Humidity: "); Serial.print(data.humidityPercent, 1); Serial.println(" %");
  Serial.print("PM1.0: "); Serial.print(data.pm1_0); Serial.println(" ug/m3");
  Serial.print("PM2.5: "); Serial.print(data.pm2_5); Serial.println(" ug/m3");
  Serial.print("PM10: "); Serial.print(data.pm10); Serial.println(" ug/m3");
  Serial.print("MQ135 Raw: "); Serial.println(data.mq135Raw);
  Serial.print("MQ135 Voltage: "); Serial.print(data.mq135Voltage, 3); Serial.println(" V");
  Serial.print("Status: "); Serial.println(levelToString(state.level));
  Serial.print("Reason: "); Serial.println(state.reason);
  Serial.print("Action Fan: "); Serial.println(state.fanOn ? "ON" : "OFF");
  Serial.print("Action HVAC: "); Serial.println(state.hvacOn ? "ON" : "OFF");
  Serial.println(makeSensorJson(data, state, sequenceNo));
}

void sendControlCommandsIfNeeded(const SensorData& data, const SystemState& state) {
  const bool changed = !state.equals(lastSentState);
  const bool repeatDue = millis() - lastCommandSentMs >= COMMAND_REPEAT_INTERVAL_MS;

  if (!firstCommandSend && !changed && !repeatDue) {
    return;
  }

  ++sequenceNo;

  String fanJson = makeCommandJson(
    "ventilation_fan",
    state.fanOn,
    state.reason.c_str(),
    "pm_mq135_humidity",
    data.pm2_5,
    "ug/m3",
    state,
    sequenceNo
  );

  String hvacJson = makeCommandJson(
    "hvac_unit",
    state.hvacOn,
    state.reason.c_str(),
    "temperature",
    data.temperatureC,
    "C",
    state,
    sequenceNo
  );

  Serial.print("[COMMAND] Fan  JSON: "); Serial.println(fanJson);
  Serial.print("[COMMAND] HVAC JSON: "); Serial.println(hvacJson);

#if ENABLE_LOCAL_HTTP_COMMANDS
  sendHttpJson(SLAVE_FAN_URL, fanJson);
  sendHttpJson(SLAVE_HVAC_URL, hvacJson);
#endif

#if ENABLE_CLOUD_UPLOAD
  String sensorJson = makeSensorJson(data, state, sequenceNo);
  sendHttpJson(CLOUD_ENDPOINT_URL, sensorJson);
#endif

  lastSentState = state;
  firstCommandSend = false;
  lastCommandSentMs = millis();
}

void setup() {
  Serial.begin(115200);
  delay(800);
  Serial.println();
  Serial.println("========== Smart Ventilation & HVAC - Week 3 ==========");

  pms.begin(PMS_BAUD, PMS_RX_PIN, PMS_TX_PIN);
  mq135.begin();
  dht22.begin();
  display.begin();
  setupWifi();

  Serial.println("[SETUP] Done. Waiting for sensor data...");
}

void loop() {
  const uint32_t now = millis();

  if (now - lastSensorReadMs >= SENSOR_READ_INTERVAL_MS) {
    lastSensorReadMs = now;
    sensorData.timestampMs = now;

    // Read sensors. PMS may fail in the first few seconds if no full frame is available yet.
    dht22.read(sensorData);
    mq135.read(sensorData);
    pms.read(sensorData);

    currentState = evaluateRules(sensorData);
    printSensorLog(sensorData, currentState);
    sendControlCommandsIfNeeded(sensorData, currentState);
  }

  if (now - lastDisplayRefreshMs >= DISPLAY_REFRESH_INTERVAL_MS) {
    lastDisplayRefreshMs = now;
    display.update(sensorData, currentState);
  }

  if (display.touched()) {
    Serial.println("[TOUCH] Touch detected. Week 3 UI only displays data; advanced touch control is for next week.");
    delay(300);
  }
}


// ESP-IDF entry point. This keeps the Arduino-style setup()/loop() flow.
extern "C" void app_main(void) {
  initArduino();
  setup();
  while (true) {
    loop();
    vTaskDelay(pdMS_TO_TICKS(1));
  }
}
