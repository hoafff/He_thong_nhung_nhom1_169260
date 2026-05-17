# Threshold Table - Tuần 3

Bảng ngưỡng này dùng cho demo tuần 3. Sau khi có dữ liệu thực tế, nhóm nên hiệu chỉnh lại.

| Thông số | Normal | Warning | Danger | Hành động |
|---|---:|---:|---:|---|
| PM2.5 | ≤ 35 µg/m³ | 36–75 µg/m³ | > 75 µg/m³ | Bật quạt/thông gió |
| Nhiệt độ | ≤ 30°C | 30–35°C | > 35°C | Bật HVAC |
| Độ ẩm | ≤ 70% | 70–85% | > 85% | Bật quạt/thông gió |
| MQ-135 raw | ≤ 1800 | 1801–2500 | > 2500 | Bật quạt/thông gió |

## Ghi chú về MQ-135

MQ-135 chưa được hiệu chuẩn để quy đổi sang ppm. Vì vậy hệ thống chỉ sử dụng giá trị `raw ADC` làm chỉ báo chất lượng không khí tương đối.

