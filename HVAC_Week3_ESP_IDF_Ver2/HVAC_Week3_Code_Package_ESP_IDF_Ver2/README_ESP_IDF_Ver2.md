# HVAC Week 3 - ESP-IDF Ver2

Bản này chuyển từ code Arduino/PlatformIO sang cấu trúc **ESP-IDF project** theo hướng dùng **Arduino as an ESP-IDF component**.

## Nội dung

```text
HVAC_Week3_Code_Package_ESP_IDF_Ver2/
├── espidf_main_controller/   # ESP32 chính: đọc sensor, xử lý luật, gửi lệnh HTTP
├── espidf_slave_fan/         # ESP32 phụ 1: nhận lệnh quạt/thông gió
├── espidf_slave_hvac/        # ESP32 phụ 2: nhận lệnh HVAC/relay
├── docs/
├── sample_logs/
└── README_ESP_IDF_Ver2.md
```

## Điểm đã sửa so với bản cũ

1. Chuyển file `.ino` sang `main.cpp` chuẩn ESP-IDF.
2. Thêm `app_main()` và gọi `initArduino()` để chạy lại `setup()` / `loop()` trong ESP-IDF.
3. Sửa các cấu hình `#if ENABLE_WIFI_AP`, `#if ENABLE_LOCAL_HTTP_COMMANDS` từ `static constexpr bool` sang macro `#define`, vì preprocessor không đọc được biến C++.
4. Bỏ phụ thuộc thư viện `DHT.h`; driver DHT22 đã được viết lại bằng Arduino GPIO timing để dễ build hơn trong ESP-IDF.
5. Phần TFT ILI9341 + XPT2046 được để dạng tùy chọn qua `ENABLE_TFT_TOUCH_UI` trong `config.h`.

## Cách nạp bằng ESP-IDF

Làm lần lượt cho từng thư mục project. Ví dụ nạp ESP32 chính:

```bash
cd espidf_main_controller
idf.py set-target esp32
idf.py build
idf.py -p COMx flash monitor
```

Với Windows, đổi `COMx` thành cổng thật, ví dụ:

```bash
idf.py -p COM5 flash monitor
```

Nạp hai ESP32 phụ tương tự:

```bash
cd espidf_slave_fan
idf.py set-target esp32
idf.py build
idf.py -p COMx flash monitor
```

```bash
cd espidf_slave_hvac
idf.py set-target esp32
idf.py build
idf.py -p COMx flash monitor
```

## Thứ tự chạy đề xuất

1. Nạp `espidf_main_controller` vào ESP32 chính.
2. Nạp `espidf_slave_fan` vào ESP32 phụ điều khiển quạt.
3. Nạp `espidf_slave_hvac` vào ESP32 phụ điều khiển HVAC/relay.
4. Cấp nguồn ESP32 chính trước để tạo Wi-Fi AP `HVAC_WEEK3`.
5. Sau đó cấp nguồn hai ESP32 phụ để chúng kết nối vào AP.

## Lưu ý về màn hình TFT/touch

Trong `espidf_main_controller/main/config.h`, mặc định:

```cpp
#define ENABLE_TFT_TOUCH_UI 0
```

Nghĩa là bản ESP-IDF này ưu tiên **build/nạp ổn định trước**, chưa bắt buộc thư viện Adafruit TFT và XPT2046.

Nếu muốn bật màn hình, đổi thành:

```cpp
#define ENABLE_TFT_TOUCH_UI 1
```

Sau đó bạn cần thêm các thư viện Arduino tương ứng vào ESP-IDF project:

- Adafruit GFX Library
- Adafruit ILI9341
- XPT2046_Touchscreen

Nếu chưa thêm các thư viện này mà bật `ENABLE_TFT_TOUCH_UI = 1`, project có thể báo lỗi thiếu header như `Adafruit_GFX.h`, `Adafruit_ILI9341.h`, `XPT2046_Touchscreen.h`.

## Lưu ý về ESP-IDF version

Bản này dùng dependency:

```yaml
dependencies:
  espressif/arduino-esp32: "*"
```

Khi build lần đầu, ESP-IDF Component Manager sẽ tự tải Arduino component. Máy cần có internet trong lần build đầu tiên.

## Nếu build lỗi do cache

Chạy:

```bash
idf.py fullclean
idf.py build
```
