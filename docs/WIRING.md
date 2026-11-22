# Osiris ESP32-S3 — Hardware Wiring Diagram & Pinout

[![English Version](https://img.shields.io/badge/Language-English-blue.svg)](WIRING.md)
[![Version Française](https://img.shields.io/badge/Langue-Français-green.svg)](CABLAGE.md)

This document details the exact hardware wiring diagram and pin assignments extracted from the firmware codebase and hardware design.

---

## 1. Pin Assignment Summary Table

| Component | Component Pin | ESP32-S3 Pin | Mode / Config | Notes |
| :--- | :--- | :--- | :--- | :--- |
| **0.96" OLED (SSD1306)** | `SDA` | **GPIO 8** | I2C Data | Hardware I2C (400 kHz Fast Mode) |
| **0.96" OLED (SSD1306)** | `SCL` | **GPIO 9** | I2C Clock | Hardware I2C (400 kHz Fast Mode) |
| **0.96" OLED (SSD1306)** | `VCC` | **3V3** | Power Output | 3.3V Power Rail |
| **0.96" OLED (SSD1306)** | `GND` | **GND** | Ground | Common Ground |
| **Top Button (BTN_TOP)** | Terminal A | **GPIO 4** | `INPUT_PULLUP` | Software internal pull-up enabled |
| | Terminal B | **GND** | Ground | Active LOW when pressed |
| **Bottom Button (BTN_BOTTOM)**| Terminal A | **GPIO 5** | `INPUT_PULLUP` | Software internal pull-up enabled |
| | Terminal B | **GND** | Ground | Active LOW when pressed |
| **Target PC (HID)** | `USB D- (DM)` | **GPIO 19** | Native USB OTG | Native USB-C Port 1 (`USB-OTG TinyUSB`) |
| **Target PC (HID)** | `USB D+ (DP)` | **GPIO 20** | Native USB OTG | Emulates HID Keyboard + Mouse |
| **Host PC (Serial/CMD)** | `UART TX` | **GPIO 43** | Serial0 TX | Routed through onboard CH343 USB-UART |
| **Host PC (Serial/CMD)** | `UART RX` | **GPIO 44** | Serial0 RX | Routed through onboard CH343 USB-UART |

---

## 2. Schematic Wiring Diagram (ASCII)

```
                       +-----------------------------------+
                       |      ESP32-S3-WROOM-1 / N16R8     |
                       |                                   |
    [ TARGET PC ] <===>| [USB-C: OTG]                      |
     (HID Keyboard/    |   (GPIO 19: D-, GPIO 20: D+)      |
      Mouse Jiggler)   |                                   |
                       |                                   |
    [ HOST PC / CMD ]<=| [USB-C: UART]                     |
     (Serial 115200    |   (GPIO 43: TX, GPIO 44: RX via   |
      Flashing/Debug)  |    onboard CH343 chip)            |
                       |                                   |
                       |       3.3V o-----------------------------o VCC [OLED Display]
                       |     GPIO 9 o-----------------------------o SCL [128x64 SSD1306]
                       |     GPIO 8 o-----------------------------o SDA (I2C 400 kHz)
                       |        GND o---+-------------------------o GND
                       |                |
                       |     GPIO 4 o---)---[ BTN_TOP ]---+    (Menu Up / Jiggler Toggle)
                       |                |                 |
                       |     GPIO 5 o---)---[ BTN_BOTTOM ]+    (Menu Down / Back)
                       |                |                 |
                       |                +-----------------+---o GND
                       +-----------------------------------+
```

---

## 3. Physical Build & Perfboard Assembly

Based on the hardware pinout diagram (`ESP32-S3 Pinouts.png`) and build photos in `pics/`:

1. **Controller Module (ESP32-S3-N16R8 DevKitC)**:
   - Dual USB Type-C ports expose native USB-OTG (`GPIO 19` / `GPIO 20`) for target HID keyboard/mouse emulation and hardware UART0 (`GPIO 43` / `GPIO 44` via onboard CH343) for host serial commands and flashing.
2. **Perfboard Integration (`pics/IMG_20260808_*.jpg`)**:
   - The ESP32-S3-N16R8 module is mounted on a hand-soldered perforated prototype board wiring the I2C bus and tactile controls directly to the microcontroller header pins.
   - The 0.96" SSD1306 OLED display (`SDA` on `GPIO 8`, `SCL` on `GPIO 9`) provides live telemetry (system clock/date, internal ESP32 die temperature, WiFi/AP status, and jiggler timer).
   - Two tactile push-buttons (`BTN_TOP` on `GPIO 4` and `BTN_BOTTOM` on `GPIO 5`, internal pull-ups active) drive on-device menu navigation while HID payloads dispatch over **Native USB OTG** or **Bluetooth LE**.

---

## 4. Hardware Interaction & Button Mappings

### Top Button (`BTN_TOP` / GPIO 4)
* **Single Tap**:
  * In Main Menu: Move cursor up (`menuIndex--`)
  * In Sub Menu: Move cursor up (`subMenuIndex--`)
* **Double Tap**:
  * In Main Menu: Enter selected sub-menu / execute direct action
  * In Sub Menu: Execute selected sub-menu action
* **Long Press**:
  * On Main Screen: Toggle Mouse Jiggler ON/OFF. Also automatically enables the Access Point (AP) if activating.

### Bottom Button (`BTN_BOTTOM` / GPIO 5)
* **Single Tap**:
  * In Main Menu: Move cursor down (`menuIndex++`)
  * In Sub Menu: Move cursor down (`subMenuIndex++`)
* **Double Tap**:
  * In Sub Menu: Return to Main Menu
  * In Main Menu: Return to Main Screen
* **Long Press**:
  * On Main Screen: Open Main Menu
