# Wi-Fi Internal Command Protocol v1 - Tuần 3

## 1. Mục tiêu

ESP32 chính gửi lệnh điều khiển đến các ESP32 phụ qua Wi-Fi nội bộ. Trong gói code tuần 3, ESP32 chính tạo SoftAP:

```text
SSID: HVAC_WEEK3
Password: 12345678
Main ESP32 IP: 192.168.4.1
```

ESP32 phụ join mạng này bằng IP tĩnh:

```text
ESP32 phụ 1 - Fan:  192.168.4.2
ESP32 phụ 2 - HVAC: 192.168.4.3
```

## 2. Endpoint

ESP32 chính gửi HTTP POST đến:

```text
http://192.168.4.2/command
http://192.168.4.3/command
```

Content-Type:

```text
application/json
```

## 3. Command payload

Ví dụ lệnh bật quạt khi PM2.5 cao:

```json
{
  "seq": 12,
  "target": "ventilation_fan",
  "command": "ON",
  "mode": "AUTO",
  "reason": "PM25_WARNING",
  "source_sensor": "pm_mq135_humidity",
  "value": 42.00,
  "unit": "ug/m3",
  "system_status": "WARNING",
  "timestamp_ms": 126000
}
```

Ví dụ lệnh bật HVAC khi nhiệt độ cao:

```json
{
  "seq": 13,
  "target": "hvac_unit",
  "command": "ON",
  "mode": "AUTO",
  "reason": "TEMP_WARNING",
  "source_sensor": "temperature",
  "value": 31.20,
  "unit": "C",
  "system_status": "WARNING",
  "timestamp_ms": 128000
}
```

## 4. Các trường dữ liệu

| Field | Kiểu | Ý nghĩa |
|---|---|---|
| `seq` | number | Số thứ tự gói lệnh |
| `target` | string | Thiết bị nhận lệnh |
| `command` | string | `ON` hoặc `OFF` |
| `mode` | string | `AUTO`, tuần sau có thể thêm `MANUAL` |
| `reason` | string | Lý do kích hoạt |
| `source_sensor` | string | Cảm biến gây ra quyết định |
| `value` | number | Giá trị cảm biến liên quan |
| `unit` | string | Đơn vị đo |
| `system_status` | string | `NORMAL`, `WARNING`, `DANGER`, `SENSOR_ERROR` |
| `timestamp_ms` | number | Thời điểm gửi theo `millis()` |

## 5. ACK payload từ ESP32 phụ

```json
{
  "target": "ventilation_fan",
  "ack": true,
  "status": "ON",
  "reason": "PM25_WARNING",
  "error": null
}
```

Nếu target sai:

```json
{
  "target": "ventilation_fan",
  "ack": false,
  "status": "OFF",
  "reason": "PM25_WARNING",
  "error": "WRONG_TARGET"
}
```

## 6. Timeout và retry

Trong code chính:

```text
HTTP timeout: 3000 ms
Repeat command interval: 10000 ms
```

Cách xử lý khi ESP32 phụ không phản hồi:

1. ESP32 chính in lỗi HTTP ra Serial Monitor.
2. Sau `COMMAND_REPEAT_INTERVAL_MS`, ESP32 chính gửi lại lệnh hiện tại.
3. Tuần sau có thể bổ sung trạng thái `SLAVE_OFFLINE` trên TFT hoặc dashboard.

