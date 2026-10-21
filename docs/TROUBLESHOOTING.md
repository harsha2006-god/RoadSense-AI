# Build log: problems we hit and what fixed them

Written down because most of the real work in this project was debugging, and these are the
things most likely to bite someone rebuilding it.

| # | Symptom | Cause / what we did |
|---|---|---|
| 1 | Arduino upload fails: `Failed to communicate with the flash chip ... exit status 2` | Appeared with the MPU6050 wired up. Checklist worked through: unplug peripherals and retry; make sure nothing is on GPIO 1/3 (UART0, used for flashing) or boot-strapping pins (0/2/12/15); lower upload speed to 115200; try a data-capable USB cable and another port; hold BOOT while uploading. Exact root cause was not recorded - isolate by removing wires first. |
| 2 | `MPU6050 not found` on ESP32 although the module's LED was lit | I2C scanner found nothing on the ESP32 **and** on an Arduino Uno with fresh wires -> module itself was dead. A replacement module appeared at `0x68`. Lesson: a lit power LED proves power, not I2C. |
| 3 | Adafruit MPU6050 library `begin()` fails but raw register reads work | Final firmware reads registers directly. Possible explanation (unverified): clone/other-chip modules report a different WHO_AM_I that the library rejects. |
| 4 | Breadboard confusion | Many breadboards split the power rails in the middle - two halves are not connected. Point-to-point jumpers remove the doubt. |
| 5 | GPS shows nothing | UART baud 9600; TX/RX must be crossed; needs outdoor sky view (first fix can take minutes). 2 -> 8 satellites once outdoors. |
| 6 | ESP32 never joins WiFi | ESP32 supports **2.4 GHz only**. Check the hotspot is not 5 GHz. |
| 7 | Firebase upload returns an error, or data appears but dashboard is empty | (a) Use the full **regional** URL (`...asia-southeast1.firebasedatabase.app`), the short `firebaseio.com` form only works for the default region. (b) Node/field names must match between firmware, simulator and dashboard (`potholes`, `latitude`, `longitude`). We lost time to a dashboard reading `events` while the firmware wrote `potholes`. |
| 8 | HTTP 401 / "Permission denied" after ~30 days | **Firebase test-mode rules expire after 30 days** and then deny everything. Publish proper rules - see `firebase/README.md`. |
| 9 | Map shows `403 Access blocked` tiles | OpenStreetMap's tile servers reject requests without a proper referrer (opening the file via `file://`). |
| 10 | Map shows "API KEY REQUIRED" watermark | CARTO's basemaps now need a key. The dashboard accepts `?carto_key=...` and otherwise falls back to a plain grid (markers still plot correctly). |
| 11 | Dashboard time column showed values like `43.9s` mixed with `1789384885.9s` | Firmware sends `millis()` (uptime) while the simulator sent epoch time. Dashboard now decodes real creation time from the Firebase push-ID instead. |
| 12 | Simulated reports did not raise confidence | Reports must fall in the same 4-decimal grid cell as the real one. The simulator now jitters around the cell centre so it cannot spill over a cell edge. |
