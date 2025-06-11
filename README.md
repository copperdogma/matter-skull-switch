# ESP32 Matter Occupancy Sensor

A Matter 1.4-compatible occupancy sensor built with ESP32-C3 SuperMini and HC-SR501 PIR sensor that integrates seamlessly with Apple Home, Google Home, and Amazon Alexa, featuring configurable timeouts and QR code setup.

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
- **Matter 1.4 Protocol:** Universal smart home integration (Apple Home, Google Home, Amazon Alexa)
- **Secure Commissioning:** QR code and manual pairing code setup
- **Power:** USB-C powered for continuous operation
- **LED Status:** Visual feedback for device status (e.g., power, motion detection, commissioning)

## Hardware Components

| Component | Details |
|-----------|---------|
| **ESP32-C3 SuperMini** | Single-core 160 MHz, 400KB SRAM, 4MB Flash, Wi-Fi, BLE 5.0 |
| **HC-SR501 PIR** | Long-range detection, wide angle lens, 5V power |

## Hardware Connections

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
   # Build for ESP32-C3
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

## Technical Specifications

- **Matter Version:** 1.4 (Universal smart home compatibility)
- **Microcontroller:** ESP32-C3 SuperMini (160 MHz, 400KB SRAM, 4MB Flash)
- **Sensor:** HC-SR501 PIR (45' range, 120° coverage)
- **Connectivity:** Wi-Fi 802.11 b/g/n, Bluetooth 5.0 LE
- **Development:** ESP-IDF v5.4.1, ESP-Matter SDK

## References

- **ESP32-C3 SuperMini:** [Pinout and specifications](https://github.com/sidharthmohannair/Tutorial-ESP32-C3-Super-Mini)
- **Matter Protocol:** [Matter 1.4 Specification](https://csa-iot.org/all-solutions/matter/)

## Author

Cam Marsollier
