# Smart Ventilation & HVAC - Code hoàn thiện Tuần 3

Gói code này dùng cho demo Tuần 3 của bài tập lớn **Hệ thống kiểm soát thông khí và HVAC thông minh**.

Mục tiêu Tuần 3:

1. ESP32 chính đọc dữ liệu từ PMS5003, MQ-135, DHT22.
2. ESP32 chính chuẩn hóa dữ liệu cảm biến thành `SensorData`.
3. ESP32 chính đánh giá ngưỡng `NORMAL / WARNING / DANGER`.
4. ESP32 chính hiển thị dữ liệu cơ bản lên màn TFT 2.8 inch.
5. ESP32 chính tạo lệnh điều khiển ESP32 phụ qua Wi-Fi nội bộ.
6. ESP32 phụ nhận lệnh HTTP JSON và bật/tắt thiết bị mô phỏng bằng LED/relay.

---

## 1. Cấu trúc thư mục

```text
HVAC_Week3_Code_Package/
├── HVAC_Week3_ESP32_Main/
│   ├── HVAC_Week3_ESP32_Main.ino
│   ├── config.h
│   ├── SensorData.h
│   ├── PMS5003Driver.h/.cpp
│   ├── MQ135Driver.h/.cpp
│   ├── DHT22Driver.h/.cpp
│   ├── RuleEngine.h/.cpp
│   ├── DisplayUI.h/.cpp
│   └── CommandProtocol.h/.cpp
│
├── HVAC_Week3_ESP32_Slave_Fan/
│   └── HVAC_Week3_ESP32_Slave_Fan.ino
│
├── HVAC_Week3_ESP32_Slave_HVAC/
│   └── HVAC_Week3_ESP32_Slave_HVAC.ino
│
├── optional_tests/
│   └── INMP441_RMS_Test/
│       └── INMP441_RMS_Test.ino
│
├── docs/
│   ├── pin_mapping_week3.md
│   ├── wifi_internal_protocol_v1.md
│   └── threshold_table.md
│
└── sample_logs/
    └── week3_expected_serial_log.txt
```

---

## 2. Thư viện cần cài trong Arduino IDE

Vào **Arduino IDE → Library Manager**, cài các thư viện sau:

```text
Adafruit GFX Library
Adafruit ILI9341
XPT2046_Touchscreen
DHT sensor library by Adafruit
Adafruit Unified Sensor
```

Các thư viện `WiFi.h`, `HTTPClient.h`, `WebServer.h` có sẵn trong ESP32 Arduino core.

---

## 3. Giả định phần cứng

Code đang giả định theo sơ đồ nhóm đã gửi:

- ESP32 DevKit làm node chính.
- PMS5003 nối UART2: `RX2 GPIO16`, `TX2 GPIO17`.
- DHT22 data: `GPIO27`.
- MQ-135 dùng **AOUT → GPIO34 ADC**.
- TFT dùng ILI9341, touch dùng XPT2046.
- ESP32 chính tạo Wi-Fi nội bộ `HVAC_WEEK3`.
- ESP32 phụ 1 có IP `192.168.4.2`, điều khiển quạt/thông gió.
- ESP32 phụ 2 có IP `192.168.4.3`, điều khiển HVAC.

Nếu chân thực tế khác, chỉ sửa file:

```text
HVAC_Week3_ESP32_Main/config.h
```

---

## 4. Lưu ý rất quan trọng về MQ-135

MQ-135 trong code này được dùng làm **chỉ báo chất lượng không khí tương đối** qua giá trị ADC raw.

Không nên ghi trong báo cáo rằng MQ-135 đo CO2 ppm chính xác nếu nhóm chưa hiệu chuẩn. Cách ghi an toàn:

> MQ-135 được dùng để đánh giá chất lượng không khí tương đối thông qua giá trị ADC; hệ thống chưa hiệu chuẩn để quy đổi chính xác sang nồng độ CO2 ppm.

Ngoài ra, theo sơ đồ nhóm, chân `DOUT` của MQ-135 có thể bị trùng với `GPIO4` của `TFT_RST`. Code này **không dùng DOUT**, chỉ dùng `AOUT → GPIO34`.

---

## 5. Cách chạy demo tuần 3

### Bước 1: Nạp code cho ESP32 chính

Mở:

```text
HVAC_Week3_ESP32_Main/HVAC_Week3_ESP32_Main.ino
```

Chọn board ESP32 Dev Module, baud upload phù hợp, sau đó upload.

Serial Monitor chọn:

```text
115200 baud
```

ESP32 chính sẽ:

- Đọc DHT22.
- Đọc PMS5003.
- Đọc MQ-135 ADC.
- Hiển thị dữ liệu lên TFT.
- Tạo Wi-Fi AP tên `HVAC_WEEK3`.
- Gửi lệnh HTTP JSON đến ESP32 phụ.

### Bước 2: Nạp code cho ESP32 phụ điều khiển quạt

Mở:

```text
HVAC_Week3_ESP32_Slave_Fan/HVAC_Week3_ESP32_Slave_Fan.ino
```

ESP32 phụ sẽ join Wi-Fi `HVAC_WEEK3`, nhận IP tĩnh:

```text
192.168.4.2
```

Khi nhận lệnh `ON/OFF`, nó bật/tắt `GPIO2`.

### Bước 3: Nạp code cho ESP32 phụ điều khiển HVAC

Mở:

```text
HVAC_Week3_ESP32_Slave_HVAC/HVAC_Week3_ESP32_Slave_HVAC.ino
```

ESP32 phụ sẽ join Wi-Fi `HVAC_WEEK3`, nhận IP tĩnh:

```text
192.168.4.3
```

Khi nhận lệnh `ON/OFF`, nó bật/tắt `GPIO2`.

---

## 6. Output cần chụp lại để nộp tuần 3

Nên lưu vào thư mục hoàn thành tuần 3:

```text
01_code/
02_serial_logs/
03_images/
04_videos/
05_protocol_docs/
README_Tuan3.md
```

Minh chứng tối thiểu:

1. Ảnh Serial Monitor đọc DHT22.
2. Ảnh Serial Monitor đọc PMS5003.
3. Ảnh Serial Monitor đọc MQ-135 raw/voltage.
4. Ảnh màn TFT hiển thị dữ liệu.
5. Log JSON command gửi từ ESP32 chính.
6. Ảnh/video ESP32 phụ bật/tắt LED hoặc relay.
7. File giao thức `wifi_internal_protocol_v1.md`.

---

## 7. Nếu code không chạy ngay

Kiểm tra theo thứ tự:

1. DHT22 có điện trở kéo lên 10k giữa DATA và 3.3V chưa.
2. PMS5003 đã cấp 5V và GND chung với ESP32 chưa.
3. PMS5003 TXD có nối vào ESP32 RX2 GPIO16 chưa.
4. MQ-135 AOUT có đảm bảo không vượt 3.3V ở chân ADC ESP32 chưa.
5. TFT có đúng driver ILI9341 không.
6. Touch có đúng chip XPT2046 không.
7. Nếu touch không nhận, sửa `TOUCH_CS` và `TOUCH_IRQ` trong `config.h`.
8. Nếu relay bật ngược, đổi `RELAY_ACTIVE_LOW = true` trong code slave.

