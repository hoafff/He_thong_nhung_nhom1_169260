#include "MQ135Driver.h"
#include "config.h"

MQ135Driver::MQ135Driver(int analogPin, int avgSamples) : _pin(analogPin), _samples(avgSamples) {}

void MQ135Driver::begin() {
  pinMode(_pin, INPUT);
#if defined(ARDUINO_ARCH_ESP32)
  analogReadResolution(12);
  analogSetPinAttenuation(_pin, ADC_11db);
#endif
}

bool MQ135Driver::read(SensorData& data) {
  long sum = 0;
  for (int i = 0; i < _samples; ++i) {
    sum += analogRead(_pin);
    delay(2);
  }

  const int raw = static_cast<int>(sum / _samples);
  data.mq135Raw = raw;
  data.mq135Voltage = (static_cast<float>(raw) / ADC_MAX_VALUE) * ADC_REF_VOLTAGE;
  data.mq135Ok = true;
  return true;
}
