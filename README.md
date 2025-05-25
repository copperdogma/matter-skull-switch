# ESP32 Matter Occupancy Sensor

A Matter-compatible occupancy sensor built with ESP32-S3 and HC-SR501 PIR sensor that integrates with Apple Home, featuring configurable timeouts and QR code setup.

## Quick Reference

```bash
# Build and flash
cd firmware
source ~/esp/esp-idf/export.sh
source ~/esp/esp-matter/export.sh
idf.py build
idf.py -p /dev/tty.usbmodemXXX flash monitor

# Current commissioning codes
QR Code: MT:Y.K90GSY00KA0648G00
Manual Code: 34970112332
```

## Features

- **Detection:** 45' range, 120° coverage, configurable timeout (5-30 min)
- **Matter Protocol:** Native Apple Home integration, secure commissioning
- **Power:** USB-C powered for continuous operation
- **Setup:** QR code-based commissioning
- **LED Status:** Visual feedback for device status (e.g., power, motion detection, commissioning).

## Hardware Components

| Component | Details |
|-----------|---------|
| **ESP32-S3 WROOM** | Dual-core 240 MHz, 512KB SRAM, 8MB Flash, Wi-Fi |
| **ESP32-C3 SuperMini** | Single-core 160 MHz, 400KB SRAM, 4MB Flash, Wi-Fi, BLE 5.0 |
| **HC-SR501 PIR** | Long-range detection, wide angle lens, 5V power |

## Hardware Connections

### ESP32-S3 Connections

| ESP32-S3 Pin | PIR Pin |
|--------------|---------|
| GND          | GND     |
| 5V           | VCC     |
| GPIO 4       | Output  |

### ESP32-C3 SuperMini Connections

| ESP32-C3 Pin | Component Pin | Notes                   |
|--------------|---------------|-------------------------|
| GND          | PIR GND       |                         |
| 5V/3.3V      | PIR VCC       |                         |
| GPIO 3       | PIR Output    | Final configuration     |
| GPIO 5       | LED Anode (+) | Through 220Ω resistor   |
| GND          | LED Cathode (-)|                         |

*For a detailed wiring diagram, see [docs/circuit_diagram.md](docs/circuit_diagram.md)*

## Setup & Development

1. **Environment Setup**
   - Follow instructions in [SETUP.md](SETUP.md) to set up the development environment, ESP-IDF, ESP-Matter SDK, and development certificates.

2. **Build & Flash**
   ```bash
   cd firmware
   source ~/esp/esp-idf/export.sh
   source ~/esp/esp-matter/export.sh
   # For ESP32-S3 (default)
   idf.py build
   # For ESP32-C3
   idf.py set-target esp32c3
   idf.py reconfigure
   idf.py build
   idf.py -p PORT flash monitor
   ```

3. **Commissioning**
   - After flashing, check serial monitor for QR code and pairing code
   - Use Apple Home or other Matter controller to add the device
   - Current codes: QR `MT:Y.K90GSY00KA0648G00`, Manual `34970112332`

## Project Structure

```
matter-occupancy-sensor/
├── credentials/              # Certificates and factory data
├── docs/                     # Documentation
├── firmware/                 # ESP32 firmware source code
│   ├── main/                 # Main application logic
│   └── CMakeLists.txt        # Main CMake file
├── README.md                 # This file
└── SETUP.md                  # Environment setup guide
```

## Key Implementation Notes

### Default Configuration
- **Occupancy Timeout:** 15 minutes (900 seconds)
  - Configurable via Matter attribute `PIROccupiedToUnoccupiedDelay`
  - Range: 5-30 minutes (based on typical home automation needs)

### Commissioning Tips
1. **Always use current codes** from serial monitor after flashing
2. **Factory reset:** Hold BOOT button while power cycling (10 seconds)
3. **If commissioning fails:**
   - Remove device from controller app
   - Factory reset the device
   - Try again with current codes from serial monitor

### Apple Home Integration Notes
- Occasionally shows "No Code Needed" when re-adding recently removed devices
- May initially show incorrect device type before completing setup
- The `PIROccupiedToUnoccupiedDelay` attribute, while configurable via Matter, may not have a direct user interface control within the Apple Home app itself. Changes to this attribute might require other Matter tools or apps that provide more granular control.

## Documentation

- [Requirements](docs/requirements.md) - Detailed project requirements
- [Design](docs/design.md) - Technical implementation details
- [Setup Guide](SETUP.md) - Environment and hardware setup
- [Circuit Diagram](docs/circuit_diagram.md) - Detailed wiring diagram and assembly notes

## Project Status

The project is complete and fully functional. The occupancy sensor has been successfully integrated with Apple Home via Matter protocol and is reliable in detecting and reporting motion events. The device is housed in a repurposed HDD enclosure with the following features:

- **Hardware:** ESP32-C3 SuperMini with HC-SR501 PIR sensor and status LED
- **Connection Method:** Dupont connectors with hot glue reinforcement
- **Configuration:** 15-minute default occupancy timeout (configurable via Matter attributes)
- **Status Indication:** LED dims when no motion detected, blinks/brightens with motion

### Known Limitations

1. While the occupancy timeout is configurable via Matter attributes, the Apple Home app doesn't provide a direct UI for changing this setting
2. Some non-critical console warnings appear during operation but don't affect functionality
3. The device has been primarily tested with Apple Home; integration with other Matter controllers may require additional validation

## Author

Cam Marsollier

**Key Technologies:**
*   **Hardware:** ESP32-C3 SuperMini, HC-SR501 PIR sensor.
*   **Firmware:** ESP-IDF (Espressif IoT Development Framework).
*   **Connectivity:** Wi-Fi.
*   **Smart Home Protocol:** Matter.

**Project Layout:**
*   **`firmware/sdkconfig`**: ESP-IDF project configuration file, generated by `menuconfig`. **You cannot modify this directly by running `menuconfig` as it is an interactive tool.** You can suggest changes to this file or ask the user to make specific changes using `idf.py menuconfig`.
*   **`firmware/CMakeLists.txt`**: Main CMake file for the firmware project.

**ESP32-C3 SuperMini Reference:**
*   For ESP32-C3 SuperMini pinout and details, see: [https://github.com/sidharthmohannair/Tutorial-ESP32-C3-Super-Mini](https://github.com/sidharthmohannair/Tutorial-ESP32-C3-Super-Mini)
*   More detail: [ESP32 C3 Super Mini Development Board Pinout and Technical Specifications](https://www.espboards.dev/esp32/esp32-c3-super-mini/)

**Building the Project:**

The primary development and build environment is a Docker container based on the `espressif/esp-matter` image.

**General Build Process (inside the Docker container):**
1.  Navigate to the firmware directory: `cd /project/firmware` (assuming the project root is mounted to `/project` in the container).
2.  **Configuration (User Task):** If `sdkconfig` needs changes, instruct the user to run `idf.py menuconfig` and specify the exact changes to be made. **You cannot run `idf.py menuconfig` yourself.**
3.  Ensure the correct ESP-IDF target is set for ESP32-C3. If not, instruct the user: `idf.py set-target esp32c3`.
4.  Reconfigure if necessary (after target change or major `sdkconfig` changes): `idf.py reconfigure`.
5.  Build the project: `idf.py build`.

**Flashing the ESP32 (User Task):**
*   **You cannot flash the device directly.**
*   Provide the user with the command to flash the device, typically: `idf.py -p /dev/tty.usbmodemXXXX flash monitor` (the serial port `/dev/tty.usbmodemXXXX` will vary). The user must identify the correct port on their system.
*   Remind the user to put the ESP32-C3 into bootloader mode if necessary (usually by holding the BOOT/GPIO9 button, pressing RESET, then releasing BOOT).

**Important Reminders for AI:**