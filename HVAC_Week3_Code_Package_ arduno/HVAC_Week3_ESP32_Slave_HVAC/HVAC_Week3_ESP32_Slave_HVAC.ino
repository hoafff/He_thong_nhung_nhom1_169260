#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

// =============================================================
//  ESP32 SLAVE 2 - HVAC / Relay prototype
//  Main ESP32 SoftAP: 192.168.4.1
//  This slave static IP: 192.168.4.3
// =============================================================

static const char* WIFI_SSID = "HVAC_WEEK3";
static const char* WIFI_PASSWORD = "12345678";

IPAddress localIP(192, 168, 4, 3);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);

static constexpr int ACTUATOR_PIN = 2;      // onboard LED or relay IN pin
static constexpr bool RELAY_ACTIVE_LOW = false;  // set true if your relay turns ON with LOW

WebServer server(80);
bool actuatorOn = false;

void writeActuator(bool on) {
  actuatorOn = on;
  if (RELAY_ACTIVE_LOW) {
    digitalWrite(ACTUATOR_PIN, on ? LOW : HIGH);
  } else {
    digitalWrite(ACTUATOR_PIN, on ? HIGH : LOW);
  }
}

String extractValue(const String& body, const String& key) {
  String pattern = "\"" + key + "\":\"";
  int start = body.indexOf(pattern);
  if (start < 0) return "";
  start += pattern.length();
  int end = body.indexOf("\"", start);
  if (end < 0) return "";
  return body.substring(start, end);
}

void handleCommand() {
  String body = server.arg("plain");
  Serial.println("[RX] " + body);

  String target = extractValue(body, "target");
  String command = extractValue(body, "command");
  String reason = extractValue(body, "reason");

  bool accepted = false;
  String error = "null";

  if (target != "hvac_unit") {
    error = "WRONG_TARGET";
  } else if (command == "ON") {
    writeActuator(true);
    accepted = true;
  } else if (command == "OFF") {
    writeActuator(false);
    accepted = true;
  } else {
    error = "UNKNOWN_COMMAND";
  }

  String ack = "{";
  ack += "\"target\":\"hvac_unit\",";
  ack += "\"ack\":" + String(accepted ? "true" : "false") + ",";
  ack += "\"status\":\"" + String(actuatorOn ? "ON" : "OFF") + "\",";
  ack += "\"reason\":\"" + reason + "\",";
  ack += "\"error\":" + (error == "null" ? "null" : "\"" + error + "\"");
  ack += "}";

  server.send(200, "application/json", ack);
  Serial.println("[ACK] " + ack);
}

void handleRoot() {
  String msg = "ESP32 Slave HVAC OK. Current state: ";
  msg += actuatorOn ? "ON" : "OFF";
  server.send(200, "text/plain", msg);
}

void setup() {
  Serial.begin(115200);
  pinMode(ACTUATOR_PIN, OUTPUT);
  writeActuator(false);

  WiFi.mode(WIFI_STA);
  WiFi.config(localIP, gateway, subnet);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to "); Serial.print(WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Slave HVAC IP: "); Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, handleRoot);
  server.on("/command", HTTP_POST, handleCommand);
  server.begin();
  Serial.println("HTTP server started: /command");
}

void loop() {
  server.handleClient();
}
