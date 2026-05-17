#include <WiFi.h>
#include <PubSubClient.h>


/*
  MVP demo for:
  Multi-room Air Quality Monitoring System

  Demo flow:
  - Simulate 2 rooms using 2 potentiometers.
  - Classify air quality by CO2 threshold.
  - Drive LEDs, buzzer and fan/relay indicator.
  - Publish telemetry to ThingsBoard by MQTT.

  Wokwi Wi-Fi:
  SSID: Wokwi-GUEST
  Password: empty
*/

// ======================= Wi-Fi / MQTT CONFIG =======================

#define WIFI_SSID       "Wokwi-GUEST"
#define WIFI_PASSWORD   ""

#define MQTT_SERVER     "demo.thingsboard.io"
#define MQTT_PORT       1883

// Replace this with your ThingsBoard device access token later
#define TB_ACCESS_TOKEN "PUT_YOUR_THINGSBOARD_DEVICE_TOKEN_HERE"

#define ENABLE_MQTT 0
#define ENABLE_WIFI 0

#define MQTT_CLIENT_ID  "esp32_gateway_air_quality_demo"
#define TB_TELEMETRY_TOPIC "v1/devices/me/telemetry"

// ======================= GPIO CONFIG =======================

// Potentiometers simulate CO2 values of each room
#define POT_ROOM_1_PIN      34
#define POT_ROOM_2_PIN      35

// Status LEDs
#define LED_GREEN_PIN       25
#define LED_YELLOW_PIN      26
#define LED_RED_PIN         27

// Local warning and actuator
#define BUZZER_PIN          32
#define FAN_RELAY_PIN       18

// Gateway status LEDs
#define LED_WIFI_PIN        19
#define LED_MQTT_PIN        23

// ======================= SYSTEM CONFIG =======================

#define TELEMETRY_INTERVAL_MS   5000UL
#define SERIAL_BAUDRATE         115200

#define CO2_NORMAL_MAX          800
#define CO2_ATTENTION_MAX       1200

#define TEMP_WARNING_MAX        35.0f
#define HUM_WARNING_MAX         80.0f

#define CO2_MIN_SIM             400
#define CO2_MAX_SIM             2000

// ======================= DATA TYPES =======================

typedef enum
{
  AIR_STATUS_NORMAL = 0,
  AIR_STATUS_ATTENTION,
  AIR_STATUS_WARNING
} air_status_t;

typedef struct
{
  const char *room_id;
  uint16_t co2_ppm;
  float temperature_c;
  float humidity_percent;
  air_status_t air_status;
  bool node_online;
} room_data_t;

// ======================= GLOBALS =======================

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

room_data_t room1;
room_data_t room2;

unsigned long lastTelemetryMs = 0;

// ======================= UTILS =======================

const char *airStatusToString(air_status_t status)
{
  switch (status)
  {
    case AIR_STATUS_NORMAL:
      return "normal";

    case AIR_STATUS_ATTENTION:
      return "attention";

    case AIR_STATUS_WARNING:
      return "warning";

    default:
      return "unknown";
  }
}

air_status_t classifyAirQuality(uint16_t co2_ppm,
                                 float temperature_c,
                                 float humidity_percent)
{
  if ((co2_ppm > CO2_ATTENTION_MAX) ||
      (temperature_c > TEMP_WARNING_MAX) ||
      (humidity_percent > HUM_WARNING_MAX))
  {
    return AIR_STATUS_WARNING;
  }

  if (co2_ppm >= CO2_NORMAL_MAX)
  {
    return AIR_STATUS_ATTENTION;
  }

  return AIR_STATUS_NORMAL;
}

uint16_t mapPotToCo2(uint8_t analogPin)
{
  int raw = analogRead(analogPin);

  if (raw < 0)
  {
    raw = 0;
  }

  if (raw > 4095)
  {
    raw = 4095;
  }

  return (uint16_t)map(raw, 0, 4095, CO2_MIN_SIM, CO2_MAX_SIM);
}

float simulateTemperature(uint16_t co2_ppm, uint8_t roomIndex)
{
  float baseTemp = (roomIndex == 1) ? 27.0f : 28.0f;
  float co2Effect = ((float)(co2_ppm - CO2_MIN_SIM)) / 500.0f;

  return baseTemp + co2Effect;
}

float simulateHumidity(uint16_t co2_ppm, uint8_t roomIndex)
{
  float baseHumidity = (roomIndex == 1) ? 58.0f : 62.0f;
  float co2Effect = ((float)(co2_ppm - CO2_MIN_SIM)) / 80.0f;

  return baseHumidity + co2Effect;
}

room_data_t buildRoomData(const char *roomId, uint8_t roomIndex, uint8_t potPin)
{
  room_data_t room;

  room.room_id = roomId;
  room.co2_ppm = mapPotToCo2(potPin);
  room.temperature_c = simulateTemperature(room.co2_ppm, roomIndex);
  room.humidity_percent = simulateHumidity(room.co2_ppm, roomIndex);
  room.air_status = classifyAirQuality(room.co2_ppm,
                                       room.temperature_c,
                                       room.humidity_percent);
  room.node_online = true;

  return room;
}

// ======================= HARDWARE CONTROL =======================

void setupPins(void)
{
  pinMode(LED_GREEN_PIN, OUTPUT);
  pinMode(LED_YELLOW_PIN, OUTPUT);
  pinMode(LED_RED_PIN, OUTPUT);

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(FAN_RELAY_PIN, OUTPUT);

  pinMode(LED_WIFI_PIN, OUTPUT);
  pinMode(LED_MQTT_PIN, OUTPUT);

  digitalWrite(LED_GREEN_PIN, LOW);
  digitalWrite(LED_YELLOW_PIN, LOW);
  digitalWrite(LED_RED_PIN, LOW);

  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(FAN_RELAY_PIN, LOW);

  digitalWrite(LED_WIFI_PIN, LOW);
  digitalWrite(LED_MQTT_PIN, LOW);
}

air_status_t getWorstStatus(room_data_t firstRoom, room_data_t secondRoom)
{
  if ((firstRoom.air_status == AIR_STATUS_WARNING) ||
      (secondRoom.air_status == AIR_STATUS_WARNING))
  {
    return AIR_STATUS_WARNING;
  }

  if ((firstRoom.air_status == AIR_STATUS_ATTENTION) ||
      (secondRoom.air_status == AIR_STATUS_ATTENTION))
  {
    return AIR_STATUS_ATTENTION;
  }

  return AIR_STATUS_NORMAL;
}

void updateLocalIndicators(room_data_t firstRoom, room_data_t secondRoom)
{
  air_status_t worstStatus = getWorstStatus(firstRoom, secondRoom);

  digitalWrite(LED_GREEN_PIN, LOW);
  digitalWrite(LED_YELLOW_PIN, LOW);
  digitalWrite(LED_RED_PIN, LOW);

  switch (worstStatus)
  {
    case AIR_STATUS_NORMAL:
      digitalWrite(LED_GREEN_PIN, HIGH);
      noTone(BUZZER_PIN);
      digitalWrite(FAN_RELAY_PIN, LOW);
      break;

    case AIR_STATUS_ATTENTION:
      digitalWrite(LED_YELLOW_PIN, HIGH);
      noTone(BUZZER_PIN);
      digitalWrite(FAN_RELAY_PIN, LOW);
      break;

    case AIR_STATUS_WARNING:
      digitalWrite(LED_RED_PIN, HIGH);
      tone(BUZZER_PIN, 2000);
      digitalWrite(FAN_RELAY_PIN, HIGH);
      break;

    default:
      noTone(BUZZER_PIN);
      digitalWrite(FAN_RELAY_PIN, LOW);
      break;
  }
}

// ======================= WIFI / MQTT =======================

void connectWiFi(void)
{
  if (WiFi.status() == WL_CONNECTED)
  {
    digitalWrite(LED_WIFI_PIN, HIGH);
    return;
  }

  Serial.print("Connecting WiFi");

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("WiFi connected. IP: ");
  Serial.println(WiFi.localIP());

  digitalWrite(LED_WIFI_PIN, HIGH);
}

void connectMQTT(void)
{
  if (mqttClient.connected())
  {
    digitalWrite(LED_MQTT_PIN, HIGH);
    return;
  }

  Serial.print("Connecting MQTT");

  while (!mqttClient.connected())
  {
    bool connected = mqttClient.connect(MQTT_CLIENT_ID, TB_ACCESS_TOKEN, NULL);

    if (connected)
    {
      Serial.println();
      Serial.println("MQTT connected to ThingsBoard.");
      digitalWrite(LED_MQTT_PIN, HIGH);
    }
    else
    {
      Serial.print(".");
      digitalWrite(LED_MQTT_PIN, LOW);
      delay(1000);
    }
  }
}

bool isFanOn(room_data_t firstRoom, room_data_t secondRoom)
{
  return (firstRoom.air_status == AIR_STATUS_WARNING) ||
         (secondRoom.air_status == AIR_STATUS_WARNING);
}

void publishTelemetry(room_data_t firstRoom, room_data_t secondRoom)
{
  if (!mqttClient.connected())
  {
    digitalWrite(LED_MQTT_PIN, LOW);
    return;
  }

  char payload[700];

  snprintf(payload,
           sizeof(payload),
           "{"
           "\"room_1_co2\":%u,"
           "\"room_1_temperature\":%.2f,"
           "\"room_1_humidity\":%.2f,"
           "\"room_1_air_status\":\"%s\","
           "\"room_1_node_online\":%s,"

           "\"room_2_co2\":%u,"
           "\"room_2_temperature\":%.2f,"
           "\"room_2_humidity\":%.2f,"
           "\"room_2_air_status\":\"%s\","
           "\"room_2_node_online\":%s,"

           "\"fan_state\":%s,"
           "\"gateway_status\":\"online\""
           "}",
           firstRoom.co2_ppm,
           firstRoom.temperature_c,
           firstRoom.humidity_percent,
           airStatusToString(firstRoom.air_status),
           firstRoom.node_online ? "true" : "false",

           secondRoom.co2_ppm,
           secondRoom.temperature_c,
           secondRoom.humidity_percent,
           airStatusToString(secondRoom.air_status),
           secondRoom.node_online ? "true" : "false",

           isFanOn(firstRoom, secondRoom) ? "true" : "false");

  bool ok = mqttClient.publish(TB_TELEMETRY_TOPIC, payload);

  if (ok)
  {
    Serial.println("Telemetry published:");
    Serial.println(payload);
  }
  else
  {
    Serial.println("Failed to publish telemetry.");
  }
}

// ======================= DEBUG =======================

void printRoomData(room_data_t room)
{
  Serial.print(room.room_id);
  Serial.print(" | CO2: ");
  Serial.print(room.co2_ppm);
  Serial.print(" ppm | Temp: ");
  Serial.print(room.temperature_c);
  Serial.print(" C | Humidity: ");
  Serial.print(room.humidity_percent);
  Serial.print(" % | Status: ");
  Serial.println(airStatusToString(room.air_status));
}

// ======================= ARDUINO MAIN =======================
void setup(void)
{
  Serial.begin(SERIAL_BAUDRATE);
  delay(1000);

  setupPins();

  Serial.println();
  Serial.println("===========================================");
  Serial.println("Multi-room Air Quality Gateway MVP Demo");
  Serial.println("ESP32 + Wokwi Offline Demo");
  Serial.println("===========================================");

  if (ENABLE_WIFI != 0)
  {
    connectWiFi();
    mqttClient.setServer(MQTT_SERVER, MQTT_PORT);

    if (ENABLE_MQTT != 0)
    {
      connectMQTT();
    }
    else
    {
      Serial.println("MQTT disabled for offline Wokwi demo.");
    }
  }
  else
  {
    Serial.println("WiFi disabled for offline Wokwi demo.");
    Serial.println("MQTT disabled for offline Wokwi demo.");
  }
}
void loop(void)
{
  if (ENABLE_WIFI != 0)
  {
    if (WiFi.status() != WL_CONNECTED)
    {
      digitalWrite(LED_WIFI_PIN, LOW);
      connectWiFi();
    }

    if (ENABLE_MQTT != 0)
    {
      if (!mqttClient.connected())
      {
        digitalWrite(LED_MQTT_PIN, LOW);
        connectMQTT();
      }

      mqttClient.loop();
    }
  }

  unsigned long nowMs = millis();

  if ((nowMs - lastTelemetryMs) >= TELEMETRY_INTERVAL_MS)
  {
    lastTelemetryMs = nowMs;

    room1 = buildRoomData("room_1", 1, POT_ROOM_1_PIN);
    room2 = buildRoomData("room_2", 2, POT_ROOM_2_PIN);

    updateLocalIndicators(room1, room2);

    Serial.println();
    Serial.println("---------- Gateway cycle ----------");
    printRoomData(room1);
    printRoomData(room2);
    Serial.print("Fan/Relay: ");
    Serial.println(isFanOn(room1, room2) ? "ON" : "OFF");

    if ((ENABLE_WIFI != 0) && (ENABLE_MQTT != 0))
    {
      publishTelemetry(room1, room2);
    }
    else
    {
      Serial.println("Offline demo mode: telemetry only printed to Serial.");
    }
  }
}