# ESP32-C3 SuperMini Matter Skull Switch

A Matter 1.4–compatible *On/Off Switch* built with the ESP32-C3 SuperMini that controls animatronic skulls and other Halloween effects. When turned "on" via Matter (Apple Home, Google Home, Amazon Alexa), the device sends a GPIO "GO!" signal to trigger an effect on connected animatronic devices.

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
https://project-chip.github.io/connectedhomeip/qrcode.html?data=MT%3AY.K90GSY00KA0648G00
```

## Features

- **Matter On/Off Switch:** Exposes a standard on/off switch controllable via Apple Home, Google Home, and Amazon Alexa
- **GPIO Control Signal:** Sends a "GO!" pulse signal to connected animatronic devices when switched "on"
- **Matter 1.4 Protocol:** Works with all major smart-home controllers
- **Secure Commissioning:** On-device QR code and manual pairing code for easy setup
- **Battery Monitoring (Future):** Plans for LiPo battery level reporting via Matter Power Source cluster
- **Compact Design:** Fits inside animatronic skulls alongside other electronics

## Hardware Components

| Component | Details |
|-----------|---------|
| **ESP32-C3 SuperMini** | Single-core 160 MHz, 400KB SRAM, 4MB Flash, Wi-Fi, BLE 5.0 |
| **GPIO Output Signal** | Triggers connected animatronic devices |
| **Status LED (optional)** | Visual feedback for device status |

## Hardware Connections

### ESP32-C3 SuperMini Connections

| ESP32-C3 Pin | Connected To | Notes                   |
|--------------|--------------|-------------------------|
| GPIO 3       | Signal Out   | "GO!" signal to animatronic skull ESP32 |
| GPIO 5       | LED Anode (+) | Through 220 Ω resistor (optional) |
| GPIO 9       | BOOT Button   | Factory reset (built-in) |
| GND          | LED Cathode (-) / Signal GND | Shared ground |
| 3.3V         | Signal VCC (if needed) | Power for signal line |

*The GPIO signal triggers the connected animatronic device to play a random audio file*

## Project Purpose

This device serves as a Matter-enabled remote trigger for animatronic Halloween decorations:

1. **Matter Controller** (Apple Home, etc.) sends "turn on" command
2. **ESP32-C3 SuperMini** receives command and activates GPIO output
3. **Connected Animatronic Device** receives "GO!" signal and plays random audio
4. **Matter Switch** can be turned "off" to stop/reset the system

The ESP32-C3 sits inside the skull along with the main animatronic controller, providing wireless Matter control for the Halloween display.

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
matter-skull-switch/
├── credentials/              # Certificates and factory data
├── docs/                     # Documentation
├── firmware/                 # ESP32 firmware source code
│   ├── main/                 # Main application logic
│   └── CMakeLists.txt        # Main CMake file
├── README.md                 # This file
└── SETUP.md                  # Environment setup guide
```

## Key Implementation Notes

### GPIO Signal Behavior
- **"ON" Command:** GPIO pin goes HIGH, sending "GO!" signal to animatronic controller
- **"OFF" Command:** GPIO pin goes LOW, stopping/resetting the signal
- **Signal Duration:** Configurable pulse duration (default: 500ms pulse)
- **Signal Voltage:** 3.3V logic level

### Matter Device Configuration
- **Device Type:** On/Off Switch
- **Endpoint:** Single endpoint with On/Off cluster
- **Power Source:** Reports battery level when LiPo power is implemented

### Commissioning Tips
1. **Always use the current QR/manual codes** printed to the serial monitor after flashing.
2. **Factory reset:** Hold the BOOT button (GPIO 9) for 5+ seconds, then release.
3. **If commissioning fails:**
   - Remove the device from the controller app.
   - Perform a factory reset.
   - Re-commission using the same codes (QR codes don't change after factory reset).

### Apple Home Integration Notes
* The device appears as a standard on/off switch in Apple Home
* Can be included in scenes and automations
* Multiple skull switches can be grouped for synchronized effects

### Factory Reset

**When to use factory reset:**
- Device shows "already added to another home" error
- Commissioning fails repeatedly
- Need to clear all pairing data and start fresh

**How to perform factory reset:**
1. **Button method:** Hold the BOOT button (GPIO 9) for 5+ seconds, then release
2. **Console method:** In serial monitor, type `factory_reset confirm` and press Enter

**What factory reset does:**
- Clears all pairing data and fabric information
- Clears Wi-Fi credentials
- **Keeps the same QR/manual codes** (this is correct behavior)
- Device will restart and be ready for new commissioning

### Future Enhancements

1. **Battery Power:** LiPo battery with USB-C charging and battery level reporting
2. **Scene Control:** Implement scene cluster for multiple trigger types
3. **Multiple GPIO Outputs:** Control multiple animatronic devices from one switch
4. **Pulse Patterns:** Different signal patterns for different effects

## Technical Specifications

- **Matter Version:** 1.4 (Universal smart home compatibility)
- **Microcontroller:** ESP32-C3 SuperMini (160 MHz, 400KB SRAM, 4MB Flash)
- **Output:** GPIO signal to animatronic controller
- **Connectivity:** Wi-Fi 802.11 b/g/n, Bluetooth 5.0 LE
- **Development:** ESP-IDF v5.4.1, ESP-Matter SDK
- **Signal Level:** 3.3V logic

## Troubleshooting

### Common Issues

**"Already added to another home" error:**
- Perform factory reset: Hold BOOT button (GPIO 9) for 5+ seconds, then release
- Remove device from Apple Home completely
- Re-add using the same QR code (codes don't change after factory reset)

**GPIO signal not working:**
- Check wiring between ESP32-C3 and animatronic controller
- Verify signal voltage levels (3.3V expected)
- Test with multimeter or oscilloscope

**Commissioning fails:**
- Check Wi-Fi credentials and network connectivity
- Ensure device is within range of your router
- Try factory reset and re-commission

**Serial monitor not connecting:**
- Check USB cable and port
- Kill any existing monitor processes: `pkill -f monitor`
- Try different USB port

## Multiple Device Setup

For controlling multiple animatronic figures:
1. Flash each ESP32-C3 with unique device identifiers
2. Commission each device separately in Apple Home
3. Group devices in scenes for synchronized effects
4. Each device can trigger its own animatronic controller independently

## References

- **ESP32-C3 SuperMini:** [Pinout and specifications](https://github.com/sidharthmohannair/Tutorial-ESP32-C3-Super-Mini)
- **Matter Protocol:** [Matter 1.4 Specification](https://csa-iot.org/all-solutions/matter/)
- **Animatronic Integration:** See docs/skull-integration.md for detailed wiring

## Author

Cam Marsollier
