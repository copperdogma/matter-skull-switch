# Scratchpad

## ✅ COMPLETED PROJECT STATUS

**Current Phase:** Project Kick-off & Documentation Migration
- ✅ Project base forked from Matter Occupancy Sensor
- ✅ Repository renamed to **matter-generic-switch** and pushed to GitHub
- ✅ ESP32-C3 SuperMini hardware selected; momentary push-button on GPIO 3
- ✅ Initial documentation pass completed (README, setup, wiring, LED, overview)

### Interim Status (post-migration)
The original occupancy-sensor firmware is still present; design documents and code are being refactored.  The new Generic Switch device type has been documented; firmware changes will follow after `idf.py menuconfig` enables **Generic Switch** and PIR code is removed.

### Detailed Documentation
- Complete circuit diagram available at [docs/circuit_diagram.md](docs/circuit_diagram.md)
- Final hardware uses ESP32-C3 SuperMini with HC-SR501 PIR sensor and status LED
- Assembly uses direct Dupont connections with hot glue reinforcement for stability
- Housed in repurposed HDD enclosure with appropriate openings for sensor and LED

---

### Hardware Configuration

**Final Hardware:**
*   Microcontroller: **ESP32-C3 SuperMini**
*   PIR Sensor: HC-SR501
*   Status LED: 5mm LED with 220Ω resistor
*   Power: USB-C to ESP32-C3 SuperMini

**Pin Configuration:**

*   **PIR Sensor Data Pin (`PIR_SENSOR_GPIO_NUM`):** GPIO 3
*   **LED Indicator Pin (`LED_INDICATOR_GPIO_NUM`):** GPIO 5

**Connections:**

1.  **HC-SR501 VCC (Power)**: Connected to ESP32-C3 5V pin
2.  **HC-SR501 GND (Ground)**: Connected to ESP32-C3 GND pin
3.  **HC-SR501 OUT (Signal Output)**: Connected to ESP32-C3 GPIO 3
4.  **Status LED Anode (+)**: Connected through 220Ω resistor to ESP32-C3 GPIO 5
5.  **Status LED Cathode (-)**: Connected to ESP32-C3 GND pin

**Powering the System:**
*   Via USB-C connection to the ESP32-C3 SuperMini board

### Current Hardware Plan

* **Microcontroller:** ESP32-C3 SuperMini
* **Input Device:** Momentary push-button (normally-open)
* **Status LED:** 5 mm LED + 220 Ω resistor (GPIO 5, optional)
* **Power:** USB-C 5 V

| Signal | ESP32-C3 Pin | Notes |
|--------|--------------|-------|
| Button VCC | 3.3 V | Internal pull-up optional |
| Button Signal | GPIO 3 | Interrupt on FALLING edge |
| Button GND | GND | |
| LED Anode | GPIO 5 | PWM brightness |
| LED Cathode | GND | |

---

### Known Limitations (pre-refactor)

1. Existing codebase still implements OccupancySensing cluster; switch cluster not yet enabled.
2. Console-log warnings from ESP-Matter remain (harmless).
3. Controller testing limited to Apple Home.

### Future Enhancement Possibilities

* OTA firmware updates
* Multi-colour LED feedback
* Battery power + deep sleep
* IP-based wake-on-press optimisations
* Enhanced security hardening

---

