#ifndef HVAC_WEEK3_CONFIG_H
#define HVAC_WEEK3_CONFIG_H

#include <Arduino.h>

// =============================================================
//  PROJECT: Smart Ventilation & HVAC - Week 3 Prototype - ESP-IDF Ver2
//  BOARD  : ESP32 DevKit
//  NOTE   : All pins follow the wiring diagram supplied by the group.
//           If your actual wire is different, only edit this file.
// =============================================================

// ---------------- Feature flags ----------------
// IMPORTANT: these are macros because the code uses #if preprocessing.
// Do not convert them to static constexpr bool.
#define ENABLE_WIFI_AP 1
#define ENABLE_WIFI_STA 0
#define ENABLE_LOCAL_HTTP_COMMANDS 1
#define ENABLE_CLOUD_UPLOAD 0

// Build-safe default for ESP-IDF.
// Set to 1 only after adding Adafruit_GFX, Adafruit_ILI9341 and XPT2046_Touchscreen libraries.
#define ENABLE_TFT_TOUCH_UI 0

// ---------------- TFT ILI9341 + XPT2046 Touch ----------------
// Display SPI pins
static constexpr int TFT_CS   = 5;
static constexpr int TFT_RST  = 4;
static constexpr int TFT_DC   = 2;
static constexpr int TFT_MOSI = 23;
static constexpr int TFT_MISO = 19;
static constexpr int TFT_SCLK = 18;

// Touch controller pins. Assumption from the diagram:
// T_DO  -> GPIO19, T_DIN -> GPIO23, T_CLK -> GPIO18,
// T_CS  -> GPIO21, T_IRQ -> GPIO22.
// If your touch module uses another CS/IRQ pin, change here.
static constexpr int TOUCH_CS  = 21;
static constexpr int TOUCH_IRQ = 22;

// ---------------- Sensors ----------------
// PMS5003 UART2
static constexpr int PMS_RX_PIN = 16;  // ESP32 RX2  <- PMS5003 TXD
static constexpr int PMS_TX_PIN = 17;  // ESP32 TX2  -> PMS5003 RXD
static constexpr uint32_t PMS_BAUD = 9600;

// DHT22
static constexpr int DHT22_PIN = 27;

// MQ-135
// IMPORTANT: use AOUT only. Do NOT connect MQ-135 DOUT to GPIO4 because
// GPIO4 is already used by TFT_RST in the current diagram.
static constexpr int MQ135_AO_PIN = 34;       // ADC input only
static constexpr float ADC_REF_VOLTAGE = 3.30f;
static constexpr int ADC_MAX_VALUE = 4095;
static constexpr int MQ135_AVG_SAMPLES = 20;

// ---------------- Thresholds for week 3 demo ----------------
// PM2.5 thresholds based on simple demo rules, not medical certification.
static constexpr int PM25_WARNING_UGM3 = 35;
static constexpr int PM25_DANGER_UGM3  = 75;

static constexpr float TEMP_WARNING_C = 30.0f;
static constexpr float TEMP_DANGER_C  = 35.0f;

static constexpr float HUMID_WARNING_PERCENT = 70.0f;
static constexpr float HUMID_DANGER_PERCENT  = 85.0f;

// MQ-135 raw ADC threshold. This is only a relative air-quality indicator.
// Calibrate these values after real testing in your room.
static constexpr int MQ135_WARNING_RAW = 1800;
static constexpr int MQ135_DANGER_RAW  = 2500;

// ---------------- Timing ----------------
static constexpr uint32_t SENSOR_READ_INTERVAL_MS = 2000;
static constexpr uint32_t DISPLAY_REFRESH_INTERVAL_MS = 2000;
static constexpr uint32_t COMMAND_REPEAT_INTERVAL_MS = 10000;

// ---------------- Local Wi-Fi command prototype ----------------
// Week 3 requirement: protocol design + prototype command sending.
// For demo without router, the main ESP32 creates a local AP and two slave ESP32s join it.
static const char* AP_SSID = "HVAC_WEEK3";
static const char* AP_PASSWORD = "12345678";  // at least 8 chars

// If ENABLE_WIFI_STA=1, fill your router Wi-Fi here.
static const char* STA_SSID = "YOUR_WIFI_SSID";
static const char* STA_PASSWORD = "YOUR_WIFI_PASSWORD";

// Slave IPs used by included slave sketches.
static const char* SLAVE_FAN_URL  = "http://192.168.4.2/command";
static const char* SLAVE_HVAC_URL = "http://192.168.4.3/command";

// Optional cloud endpoint placeholder. Not required for week 3.
static const char* CLOUD_ENDPOINT_URL = "https://example.com/api/sensors";

#endif
