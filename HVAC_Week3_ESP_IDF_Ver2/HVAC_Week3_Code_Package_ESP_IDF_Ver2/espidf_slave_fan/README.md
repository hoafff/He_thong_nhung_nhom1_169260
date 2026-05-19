# Cách build / flash project này bằng ESP-IDF

Mở terminal ESP-IDF tại đúng thư mục project này, rồi chạy:

```bash
idf.py set-target esp32
idf.py build
idf.py -p COMx flash monitor
```

Trên Windows, thay `COMx` bằng cổng thật, ví dụ `COM5`.

Nếu build lần đầu, ESP-IDF sẽ tải Arduino component theo file `main/idf_component.yml`, nên máy cần có internet.
