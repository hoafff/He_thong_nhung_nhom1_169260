#include "DHT22Driver.h"

DHT22Driver::DHT22Driver(int dataPin) : _dht(dataPin, DHT22) {}

void DHT22Driver::begin() {
  _dht.begin();
}

bool DHT22Driver::read(SensorData& data) {
  float h = _dht.readHumidity();
  float t = _dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    data.dhtOk = false;
    return false;
  }

  data.temperatureC = t;
  data.humidityPercent = h;
  data.dhtOk = true;
  return true;
}
