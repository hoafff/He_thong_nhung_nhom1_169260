# Pin Mapping Tuần 3

Bảng này dùng cho ESP32 chính theo sơ đồ nhóm cung cấp.

## 1. PMS5003

| PMS5003 | ESP32 | Ghi chú |
|---|---:|---|
| VCC | 5V | Cấp nguồn cảm biến |
| GND | GND | GND chung |
| TXD | GPIO16 / RX2 | PMS gửi dữ liệu sang ESP32 |
| RXD | GPIO17 / TX2 | ESP32 gửi lệnh sang PMS, có thể vẫn nối dù code hiện chỉ đọc |

## 2. DHT22

| DHT22 | ESP32 | Ghi chú |
|---|---:|---|
| VCC | 3.3V | Nên dùng 3.3V |
| GND | GND | GND chung |
| DATA | GPIO27 | Cần điện trở pull-up 10k lên 3.3V |

## 3. MQ-135

| MQ-135 | ESP32 | Ghi chú |
|---|---:|---|
| VCC | 5V | Module MQ thường cần 5V cho heater |
| GND | GND | GND chung |
| AOUT | GPIO34 | Dùng ADC để đọc chất lượng không khí tương đối |
| DOUT | Không dùng | Tránh trùng GPIO4 với TFT_RST |

Cảnh báo: AOUT của module cấp 5V có thể vượt 3.3V. Cần đảm bảo tín hiệu vào GPIO34 không vượt 3.3V, ví dụ dùng cầu chia áp hoặc mạch chuyển mức.

## 4. TFT ILI9341

| TFT | ESP32 |
|---|---:|
| VCC | 5V |
| GND | GND |
| CS | GPIO5 |
| RST | GPIO4 |
| DC | GPIO2 |
| MOSI | GPIO23 |
| MISO / SDO | GPIO19 |
| SCK | GPIO18 |
| LED | 3.3V |

## 5. Touch XPT2046

| Touch | ESP32 |
|---|---:|
| T_DO | GPIO19 |
| T_DIN | GPIO23 |
| T_CLK | GPIO18 |
| T_CS | GPIO21 |
| T_IRQ | GPIO22 |

Nếu sơ đồ thực tế của nhóm dùng chân khác, sửa trong `config.h`.

## 6. INMP441

Tuần 3 chưa tích hợp AI voice vào firmware chính. Có test riêng trong `optional_tests`.

| INMP441 | ESP32 |
|---|---:|
| VDD/VIN | 3.3V |
| GND | GND |
| SCK/BCLK | GPIO26 |
| WS/LRCLK | GPIO25 |
| SD/DOUT | GPIO33 |
| L/R | GND |

