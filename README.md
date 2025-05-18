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

## Hardware Components

| Component | Details |
|-----------|---------|
| **ESP32-S3 WROOM** | Dual-core 240 MHz, 512KB SRAM, 8MB Flash, Wi-Fi |
| **HC-SR501 PIR** | Long-range detection, wide angle lens, 5V power |

## Hardware Connections

| ESP32-S3 Pin | PIR Pin |
|--------------|---------|
| GND          | GND     |
| 5V           | VCC     |
| GPIO 7*      | Output  |

*GPIO pin can be configured in menuconfig

## Setup & Development

1. **Environment Setup**
   - Follow instructions in [SETUP.md](SETUP.md) to set up the development environment, ESP-IDF, ESP-Matter SDK, and development certificates.

2. **Build & Flash**
   ```bash
   cd firmware
   source ~/esp/esp-idf/export.sh
   source ~/esp/esp-matter/export.sh
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

## Documentation

- [Requirements](docs/requirements.md) - Detailed project requirements
- [Design](docs/design.md) - Technical implementation details
- [Setup Guide](SETUP.md) - Environment and hardware setup

## Author

Cam Marsollier