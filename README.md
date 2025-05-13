# ESP32 Matter Occupancy Sensor

A reliable, Matter-compatible occupancy sensor designed for garage monitoring that seamlessly integrates with Apple Home. Built with an ESP32-S3 and HC-SR501 PIR sensor, powered by USB-C, and configured via QR code.

## Features

- **Reliable Detection**
  - 45' range with 120° coverage angle
  - Optimized for person-door area monitoring
  - Configurable occupancy timeout (5-30 minutes)
  - False positive mitigation for garage environment

- **Matter Integration**
  - Native Apple Home compatibility
  - QR code-based setup
  - Secure commissioning
  - Real-time occupancy status updates

- **Simple Setup**
  - USB-C powered
  - Dual ports for power and programming
  - Tool-free mounting options
  - Automatic light control integration

## Hardware

- ESP32-S3 WROOM
  - Dual-core 240 MHz processor
  - 512KB SRAM, 8MB Flash
  - Built-in Wi-Fi for Matter connectivity
  - Dual USB-C ports

- HC-SR501 PIR Sensor
  - Maximum sensitivity for long-range detection
  - Wide angle lens for optimal coverage
  - Direct 5V power from ESP32
  - EMI shielded for garage environment

## Firmware Details

**IMPORTANT: Before proceeding with firmware development, ensure you have fully completed the environment and base firmware setup as detailed in `SETUP.MD` in the project root.**

This project aims to create a Matter-enabled Occupancy Sensor using an ESP32-S3. It is based on the `esp-matter/examples/sensors` example, which also demonstrates temperature and humidity sensors (SHTC3) alongside an occupancy sensor (PIR).

This application, by default (from the base example), creates the temperature sensor, humidity sensor, and occupancy sensor on endpoint 1, 2, and 3 respectively. For this project, we will primarily focus on the occupancy sensor functionality.

See the [ESP-Matter Developing Guide](https://docs.espressif.com/projects/esp-matter/en/latest/esp32/developing.html) for more information about building, flashing, and customizing Matter firmware.

### Connecting the Sensors (Example Configuration - Needs ESP32-S3 Adaptation)

The following connection details are from the original `sensors` example and are specified for an **ESP32-C3**.

**For this ESP32-S3 based Occupancy Sensor project, you will need to:**
1.  Select appropriate GPIO pins on your ESP32-S3 board for your chosen PIR/occupancy sensor.
2.  If using other sensors (like the SHTC3), select appropriate GPIOs for them.
3.  Configure these GPIO assignments in `idf.py menuconfig` under the relevant driver or application settings.

**Original Example Wiring (ESP32-C3):**

- Connecting the SHTC3, temperature and humidity sensor:

| ESP32-C3 Pin | SHTC3 Pin |
|--------------|-----------|
| GND          | GND       |
| 3V3          | VCC       |
| GPIO 4       | SDA       |
| GPIO 5       | SCL       |

- Connecting the PIR sensor:

| ESP32-C3 Pin | PIR Pin |
|--------------|---------|
| GND          | GND     |
| 3V3          | VCC     |
| GPIO 7       | Output  |

**_NOTE (from original example):_**
- Above mentioned wiring connection is configured by default in the example.
- Ensure that the GPIO pins used for the sensors are correctly configured through menuconfig.
- Modify the configuration parameters as needed for your specific hardware setup.

### Occupancy Sensor Implementation Details

*(This section will be updated with project-specific hardware choices, pin configurations for ESP32-S3, and any custom logic for the occupancy sensor.)*

## Development Setup

1. Install Prerequisites
   - ESP-IDF v5.1+ (Ensure environment variables are set, see `SETUP.MD`)
   - Matter SDK (As configured during setup, see `SETUP.MD`)
   - Development certificates (As configured during setup, see `SETUP.MD`)

2. Clone Repository (if not already done)
   ```bash
   git clone [repository-url]
   cd esp32-matter-occupancy
   ```

3. Build Environment & Configuration
   ```bash
   cd firmware
   # The target should already be set via ../SETUP.MD, but can be confirmed/set:
   idf.py set-target esp32s3
   # Configure project-specific settings (like Wi-Fi, Matter settings, GPIOs)
   idf.py menuconfig
   ```

4. Build & Flash
   ```bash
   idf.py build
   idf.py -p [PORT] flash monitor # Flash and start monitoring serial output
   ```
   *(Replace `[PORT]` with your ESP32-S3's serial port, e.g., `/dev/tty.usbmodem12345` or `COM3`)*

## Usage (Testing with chip-tool)

These instructions are for testing the sensor clusters using `chip-tool`.

- Commission the app using a Matter controller.
- Read the attributes.

Below, we are using `chip-tool` to commission and subscribe the sensor attributes.
```bash
# Commission (replace with your device's actual discriminator and setup PIN if different)
# Ensure your device is in commissioning mode (e.g., via a button press if implemented)
# Run chip-tool from the connected-home-ip build directory
chip-tool pairing ble-wifi 1 YOUR_SSID YOUR_PASSPHRASE 20202021 3840

# Start chip-tool in interactive mode
chip-tool interactive start

# Subscribe to attributes (adjust endpoint IDs if your final application differs)
# Endpoint 1: Temperature Sensor (from example - may not be active in final build)
> temperaturemeasurement subscribe measured-value 3 10 1 1
# Endpoint 2: Humidity Sensor (from example - may not be active in final build)
> relativehumiditymeasurement subscribe measured-value 3 10 1 2
# Endpoint 3: Occupancy Sensor (primary for this project)
> occupancysensing subscribe occupancy 3 10 1 3
```

## Project Structure

```
esp32-matter-occupancy/
├── docs/                    # Documentation
│   ├── requirements.md      # Requirements (replaces spec.md)
│   ├── design.md            # Technical design
│   └── CHANGELOG.md         # Project history
│
├── firmware/                # ESP32 firmware source code
│   ├── main/                # Main application logic
│   └── CMakeLists.txt       # Main CMake file for firmware
│
├── credentials/             # Certificates and keys (see SETUP.MD)
├── build/                   # Build output (ignored by git)
├── managed_components/      # ESP-IDF managed components (ignored by git)
├── .vscode/                 # VSCode settings
├── .gitignore
├── README.md                # This file
└── SETUP.md                 # Initial environment setup guide
```
*(Note: Project structure may evolve)*

## Documentation

- [Requirements](docs/requirements.md) - Detailed project requirements
- [Design Decisions](docs/design.md) - Technical implementation details
- [Changelog](docs/CHANGELOG.md) - Project history and updates
- [Setup Guide](SETUP.md) - Initial environment and hardware setup

## Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## License

[License Type] - *TODO: Add license file and update here.*

## Author

Cam Marsollier 