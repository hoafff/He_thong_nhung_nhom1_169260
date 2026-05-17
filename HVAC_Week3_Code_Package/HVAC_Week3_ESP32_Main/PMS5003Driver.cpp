#include "PMS5003Driver.h"

PMS5003Driver::PMS5003Driver(HardwareSerial& serialPort) : _serial(serialPort) {}

void PMS5003Driver::begin(uint32_t baud, int rxPin, int txPin) {
  _serial.begin(baud, SERIAL_8N1, rxPin, txPin);
  _serial.setTimeout(150);
}

uint16_t PMS5003Driver::u16(const uint8_t highByte, const uint8_t lowByte) {
  return (static_cast<uint16_t>(highByte) << 8) | lowByte;
}

bool PMS5003Driver::readFrame(uint8_t frame[32]) {
  // PMS5003 frame: 32 bytes, header 0x42 0x4D, checksum over bytes 0..29.
  while (_serial.available() > 0) {
    if (_serial.peek() != 0x42) {
      _serial.read();
      continue;
    }

    if (_serial.available() < 32) {
      return false;
    }

    size_t n = _serial.readBytes(frame, 32);
    if (n != 32) return false;

    if (frame[0] != 0x42 || frame[1] != 0x4D) {
      continue;
    }

    const uint16_t frameLength = u16(frame[2], frame[3]);
    if (frameLength != 28) {
      continue;
    }

    uint16_t checksum = 0;
    for (int i = 0; i < 30; ++i) checksum += frame[i];
    const uint16_t receivedChecksum = u16(frame[30], frame[31]);
    if (checksum != receivedChecksum) {
      continue;
    }

    return true;
  }

  return false;
}

bool PMS5003Driver::read(SensorData& data) {
  uint8_t frame[32];
  if (!readFrame(frame)) {
    data.pmsOk = false;
    return false;
  }

  // Use atmospheric environment values, more suitable for ambient monitoring.
  data.pm1_0 = u16(frame[10], frame[11]);
  data.pm2_5 = u16(frame[12], frame[13]);
  data.pm10  = u16(frame[14], frame[15]);
  data.pmsOk = true;
  return true;
}
