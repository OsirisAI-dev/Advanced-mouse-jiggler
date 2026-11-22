# Osiris ESP32-S3 — Advanced Multi-Transport HID & Companion Device

[![English Version](https://img.shields.io/badge/Language-English-blue.svg)](README.md)
[![Version Française](https://img.shields.io/badge/Langue-Français-green.svg)](README_FR.md)

Osiris is an open-source, dual-transport (Native USB OTG & Bluetooth Low Energy) human interface device (HID) controller, mouse jiggler, and hardware telemetry dashboard powered by the **ESP32-S3** (N16R8).

It combines physical on-device control (0.96" OLED display with a 2-button navigation menu) with a wireless web application served directly from the ESP32-S3's internal memory via an onboard WiFi Access Point or local network.

---

## Key Features

* **Dual-Transport HID**:
  * **Native USB OTG**: Hardware USB HID keyboard and mouse emulation via the ESP32-S3 internal USB PHY (TinyUSB).
  * **Bluetooth Low Energy (BLE)**: Wireless keyboard emulation for mobile devices, tablets, and PCs.
  * **Intelligent Transport Arbitration**: Automatically disables BLE when a physical USB HID host is plugged in ("USB or BLE, never both").
* **Undetectable Mouse Jiggler**:
  * Micro-movements every 2 seconds with configurable timeout durations (30 min, 60 min, 90 min, or unlimited).
  * 100% non-blocking state machine architecture (zero frame drops or button lag).
* **On-Device Physical UI**:
  * 0.96" monochrome I2C OLED display (SSD1306, 128x64) running at 400 kHz fast mode.
  * 2-button multi-click gesture navigation (`OneButton`: single tap, double tap, long press).
  * Dynamic Grid or List menu layouts with smooth viewport scrolling.
  * Multilingual interface support: English, French, and German.
  * Hardware power-down sleep (`SSD1306_DISPLAYOFF`, < 10 µA) with activity wake-up.
* **Embedded Web Application & WebSocket API**:
  * Hosted directly from flash (`index_html.h`) via `ESPAsyncWebServer`.
  * Real-time bidirectional WebSocket transport (`/ws`) for instant synchronization.
  * Live dashboard displaying PC time/date, internal ESP32 die temperature, network status, and client count.
  * **Remote Keyboard**: Live typing and macro injection (e.g., `CTRL+ALT+DEL`, `WIN+R`, `ALT+TAB`, function keys).
  * **NVS Password Vault**: Store up to 8 credential slots in flash memory; injects username, Tab, and password via active HID with one click.
  * **WiFi Scanner & Station Mode**: Scan local 2.4 GHz WiFi networks, view signal strength (RSSI) / encryption, and connect to local routers.
  * **mDNS Support**: Access the device directly via `http://osiris-device.local`.

---

## Hardware Architecture & Pinout

For complete circuit schematics, ASCII diagrams, and integration notes, please read [WIRING.md](WIRING.md).

![Osiris ESP32-S3 Pinouts](ESP32-S3%20Pinouts.png)

| Component | Component Pin | ESP32-S3 Pin | Function |
| :--- | :--- | :--- | :--- |
| **0.96" OLED (SSD1306)** | `SDA` | **GPIO 8** | I2C Data (400 kHz Fast Mode) |
| **0.96" OLED (SSD1306)** | `SCL` | **GPIO 9** | I2C Clock (400 kHz Fast Mode) |
| **0.96" OLED (SSD1306)** | `VCC` | **3.3V** | Power Rail |
| **0.96" OLED (SSD1306)** | `GND` | **GND** | Ground |
| **Top Button (`BTN_TOP`)** | Pin 1 to GPIO, Pin 2 to GND | **GPIO 4** | Active LOW, Internal Pull-Up |
| **Bottom Button (`BTN_BOTTOM`)** | Pin 1 to GPIO, Pin 2 to GND | **GPIO 5** | Active LOW, Internal Pull-Up |
| **Target PC (HID)** | Native USB Type-C Port | **GPIO 19 (D-) / GPIO 20 (D+)** | Native USB OTG (TinyUSB) |
| **Host PC (Serial/CMD)** | UART USB Type-C Port | **GPIO 43 (TX) / GPIO 44 (RX)** | CH343 / CH340 Bridge (115200 baud) |

---

## Repository Structure

```
Osiris-ESP32S3/
├── firmware/
│   └── Osiris/
│       ├── Osiris.ino          # Main sketch & UI state machine
│       ├── ble_keys.cpp        # BLE HID keyboard implementation
│       ├── ble_keys.h
│       ├── usb_keys.cpp        # Native USB OTG keyboard/mouse implementation
│       ├── usb_keys.h
│       ├── net_scan.cpp        # WiFi network scanner
│       ├── net_scan.h
│       ├── index_html.h        # Embedded web application (HTML/CSS/JS)
│       ├── partitions.csv      # 16MB flash partition table
│       ├── setup.txt           # Arduino IDE configuration reference (EN)
│       └── setup_FR.txt        # Arduino IDE configuration reference (FR)
├── pics/                       # Photos of the hardware perfboard build
│   ├── README.md
│   ├── README_FR.md
│   └── IMG_20260808_*.jpg      # Front OLED & back perfboard solder views
├── docs/
│   ├── WIRING.md               # Detailed schematics and pinout reference (EN)
│   └── CABLAGE.md              # Detailed schematics and pinout reference (FR)
├── ESP32-S3 Pinouts.png        # Annotated ESP32-S3 perfboard pinout diagram
├── WIRING.md                   # Quick access wiring reference (EN)
├── CABLAGE.md                  # Quick access wiring reference (FR)
├── README.md                   # Main documentation (EN)
├── README_FR.md                # Main documentation (FR)
├── LICENSE                     # MIT License
└── .gitignore
```

---

## Compilation & Flashing Guide

### 1. Arduino IDE Setup
1. Install **Arduino IDE** (v2.x recommended).
2. Add ESP32 board support via the Boards Manager:
   * URL: `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
   * Install `esp32` by Espressif Systems (v2.0.14 or later).

### 2. Required Libraries
Install the following libraries via the Arduino Library Manager (`Ctrl+Shift+I`):
* `OneButton` by Matthias Hertel
* `Adafruit SSD1306` & `Adafruit GFX Library`
* `AsyncTCP` and `ESPAsyncWebServer`
* `ESP32-BLE-Keyboard` by T-vK (available via GitHub)

### 3. Board Configuration Settings
Open `firmware/Osiris/Osiris.ino` and configure the **Tools** menu in Arduino IDE:

| Setting | Required Value | Explanation |
| :--- | :--- | :--- |
| **Board** | `ESP32S3 Dev Module` | Standard ESP32-S3 module |
| **Flash Size** | `16MB (128Mb)` | Matches N16 flash |
| **Partition Scheme** | `16M Flash (3MB APP/9.9MB FATFS)` *(or `Huge APP (3MB No OTA/1MB SPIFFS)`)* | Raises IDE sketch limit from 1.25 MB to 3 MB; `firmware/Osiris/partitions.csv` supplies the custom 16 MB partition layout |
| **PSRAM** | `OPI PSRAM` | Matches R8 octal SPI PSRAM |
| **USB Mode** | `USB-OTG (TinyUSB)` | **Required**: Enables USB HID keyboard & mouse |
| **USB CDC On Boot** | `Disabled` | **Required**: Keeps Serial bound to hardware UART0 (CH343 port) |
| **Upload Speed** | `921600` | Standard fast flashing speed |

---

## Default Access & Credentials

The firmware is pre-configured with generic mock credentials:

* **Access Point (AP) SSID**: `Osiris_AP`
* **Access Point (AP) Password**: `Password1234`
* **Web UI URL**: `http://192.168.4.1` (or `http://osiris-device.local` via mDNS)
* **Serial Baud Rate**: `115200 baud` (via UART USB port)

Credentials can be customized at any time in `firmware/Osiris/Osiris.ino` or configured dynamically via the web portal.

---

## License

This project is licensed under the [MIT License](LICENSE).
