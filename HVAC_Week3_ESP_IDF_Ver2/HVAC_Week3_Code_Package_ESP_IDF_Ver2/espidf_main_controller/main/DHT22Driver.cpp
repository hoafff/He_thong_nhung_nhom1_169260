#include "DHT22Driver.h"

DHT22Driver::DHT22Driver(int dataPin) : _pin(dataPin) {}

void DHT22Driver::begin() {
  pinMode(_pin, INPUT_PULLUP);
}

bool DHT22Driver::waitForLevel(uint8_t level, uint32_t timeoutUs) {
  const uint32_t start = micros();
  while (digitalRead(_pin) != level) {
    if ((uint32_t)(micros() - start) > timeoutUs) {
      return false;
    }
    yield();
  }
  return true;
}

bool DHT22Driver::read(SensorData& data) {
  uint8_t bytes[5] = {0, 0, 0, 0, 0};

  // DHT22 start signal: MCU pulls bus LOW for at least 1 ms, then releases it.
  pinMode(_pin, OUTPUT);
  digitalWrite(_pin, LOW);
  delayMicroseconds(1200);
  digitalWrite(_pin, HIGH);
  delayMicroseconds(35);
  pinMode(_pin, INPUT_PULLUP);

  // Sensor response: LOW ~80 us, HIGH ~80 us, then data starts.
  if (!waitForLevel(LOW, 100))  { data.dhtOk = false; return false; }
  if (!waitForLevel(HIGH, 100)) { data.dhtOk = false; return false; }
  if (!waitForLevel(LOW, 100))  { data.dhtOk = false; return false; }

  for (int i = 0; i < 40; ++i) {
    if (!waitForLevel(HIGH, 100)) { data.dhtOk = false; return false; }

    const uint32_t highStart = micros();
    if (!waitForLevel(LOW, 120)) { data.dhtOk = false; return false; }
    const uint32_t highTime = micros() - highStart;

    const uint8_t bit = highTime > 45 ? 1 : 0;
    bytes[i / 8] <<= 1;
    bytes[i / 8] |= bit;
  }

  const uint8_t checksum = bytes[0] + bytes[1] + bytes[2] + bytes[3];
  if (checksum != bytes[4]) {
    data.dhtOk = false;
    return false;
  }

  const uint16_t rawHumidity = ((uint16_t)bytes[0] << 8) | bytes[1];
  uint16_t rawTemp = ((uint16_t)bytes[2] << 8) | bytes[3];

  const bool negative = (rawTemp & 0x8000) != 0;
  rawTemp &= 0x7FFF;

  data.humidityPercent = rawHumidity / 10.0f;
  data.temperatureC = (negative ? -1.0f : 1.0f) * (rawTemp / 10.0f);
  data.dhtOk = true;
  return true;
}
