#include <Arduino.h>
#include "driver/i2s.h"

// Optional Week 3 test for INMP441 microphone.
// This only prints RMS audio level. Keyword/AI voice control is for later weeks.

static constexpr int I2S_SCK = 26;  // BCLK / SCK
static constexpr int I2S_WS  = 25;  // LRCLK / WS
static constexpr int I2S_SD  = 33;  // DOUT / SD

static constexpr i2s_port_t I2S_PORT = I2S_NUM_0;
static constexpr int SAMPLE_RATE = 16000;
static constexpr int BUFFER_SAMPLES = 512;

void setupI2S() {
  const i2s_config_t i2s_config = {
    .mode = static_cast<i2s_mode_t>(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 4,
    .dma_buf_len = BUFFER_SAMPLES,
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0
  };

  const i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SCK,
    .ws_io_num = I2S_WS,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = I2S_SD
  };

  i2s_driver_install(I2S_PORT, &i2s_config, 0, nullptr);
  i2s_set_pin(I2S_PORT, &pin_config);
  i2s_zero_dma_buffer(I2S_PORT);
}

void setup() {
  Serial.begin(115200);
  delay(800);
  setupI2S();
  Serial.println("INMP441 RMS test started");
}

void loop() {
  int32_t samples[BUFFER_SAMPLES];
  size_t bytesRead = 0;
  i2s_read(I2S_PORT, samples, sizeof(samples), &bytesRead, portMAX_DELAY);

  int count = bytesRead / sizeof(int32_t);
  double sumSquares = 0;
  for (int i = 0; i < count; ++i) {
    int32_t s = samples[i] >> 14; // scale down 32-bit I2S sample
    sumSquares += static_cast<double>(s) * static_cast<double>(s);
  }

  double rms = sqrt(sumSquares / max(count, 1));
  Serial.print("Mic RMS: ");
  Serial.println(rms, 2);
  delay(200);
}
