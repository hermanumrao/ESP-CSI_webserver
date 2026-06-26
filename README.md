# ESP32 CSI Web Monitor

A minimal ESP-IDF application that turns an ESP32 into a live Wi-Fi sensing dashboard. The device connects to your Wi-Fi network, captures **Channel State Information (CSI)** and **RSSI** from incoming Wi-Fi frames, and serves a self-hosted web page that visualizes the data in real time — no app or external server required.

Open `http://<ESP32-IP>/` in any browser on the same network and watch the signal data update live.

## How It Works

1. The ESP32 connects to a Wi-Fi access point using credentials set via `idf.py menuconfig`.
2. CSI capture is enabled on the Wi-Fi radio (`esp_wifi_set_csi`), and a callback (`wifi_csi_rx_cb`) stores the latest RSSI and CSI buffer in memory whenever a frame is received.
3. A background task periodically queries the AP info, which helps keep frames flowing so CSI samples are generated continuously.
4. A lightweight HTTP server (`esp_http_server`) exposes two endpoints:
   - `GET /` — serves a single-page HTML/JS dashboard
   - `GET /data` — returns the latest RSSI and CSI buffer as JSON
5. The web page polls `/data` every 200 ms and updates the displayed RSSI value and raw CSI byte stream.

## Features

- Real-time RSSI display
- Live raw CSI data stream rendered in-browser
- Self-contained web server — no companion app, cloud service, or extra hardware
- Works over your existing Wi-Fi network
- Lightweight enough to run continuously on a single ESP32

## Hardware Required

- An ESP32 development board with CSI support (ESP32, ESP32-S2, ESP32-C3, ESP32-S3, ESP32-C5, ESP32-C6, or ESP32-C61), e.g. ESP32-DevKitC or ESP-WROVER-KIT
- A USB cable for power and programming
- A Wi-Fi access point for the board to connect to

## Software Required

- [ESP-IDF](https://github.com/espressif/esp-idf) (v4.4.1 or later)
- [esp_csi_gain_ctrl](https://github.com/espressif/esp-csi) component (pulled in automatically via the IDF Component Manager)

## Getting Started

### 1. Clone the repository

```bash
git clone https://github.com/hermanumrao/ESP-CSI_webserver.git
cd ESP-CSI_webserver
```

### 2. Set your target chip

```bash
idf.py set-target <chip_name>
```

Replace `<chip_name>` with `esp32`, `esp32s3`, `esp32c6`, etc., depending on your board.

### 3. Configure Wi-Fi credentials

```bash
idf.py menuconfig
```

Navigate to **Example Connection Configuration** and:
- Set "Connect using" to **Wi-Fi**
- Enter your network's **SSID** and **password** (leave the password blank for an open network)

### 4. Build, flash, and monitor

```bash
idf.py -p PORT build flash monitor
```

Replace `PORT` with your board's serial port (e.g. `/dev/ttyUSB0` on Linux, `COM3` on Windows).

Once connected, the serial monitor will print the device's IP address:

```
I (xxxx) CSI_WEB: ESP32 IP: 192.168.x.x
```

To exit the monitor, press `Ctrl-]`.

### 5. View the dashboard

Open a browser on a device connected to the same network and navigate to:

```
http://<ESP32-IP>/
```

You should see the RSSI value and a live stream of CSI data updating roughly every 200 ms.

## Project Structure

```
ESP-CSI_webserver/
├── CMakeLists.txt          # Top-level project build configuration
├── sdkconfig.defaults      # Default Wi-Fi / CSI / UART configuration
└── main/
    ├── app_main.c           # Wi-Fi setup, CSI capture, and HTTP server logic
    ├── CMakeLists.txt       # Component build configuration
    └── idf_component.yml    # Component dependencies (IDF, esp_csi_gain_ctrl)
```

## Notes

- CSI configuration differs slightly between chip targets; `app_main.c` selects the appropriate config struct automatically based on `CONFIG_IDF_TARGET_ESP32C6` vs. other targets.
- The CSI buffer is capped at 256 bytes (`MAX_CSI_LEN`) per sample for simplicity.
- This project builds on Espressif's `recv_router_csi` example, replacing the serial-only output with a live web dashboard.

## References

- [esp-csi](https://github.com/espressif/esp-csi) — Espressif's official CSI examples and components
- [esp-idf](https://github.com/espressif/esp-idf) — Espressif IoT Development Framework
