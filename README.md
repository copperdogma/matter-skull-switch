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

While the base `esp-matter/examples/sensors` example demonstrates multiple sensors (temperature, humidity, occupancy) on different endpoints, this project is streamlined to focus **only on the occupancy sensor functionality, which will reside on Endpoint 1.**

For detailed information about building, flashing, and customizing Matter firmware, refer to the official documentation:
- **ESP-IDF v5.4.1 Programming Guide (ESP32-S3):** [https://docs.espressif.com/projects/esp-idf/en/v5.4.1/esp32s3/](https://docs.espressif.com/projects/esp-idf/en/v5.4.1/esp32s3/)
- **ESP-Matter Developing Guide (for `main` branch):** [https://docs.espressif.com/projects/esp-matter/en/latest/esp32/developing.html](https://docs.espressif.com/projects/esp-matter/en/latest/esp32/developing.html)
- **ESP32-S3 Hardware Resources (Datasheet, TRM):** [https://www.espressif.com/en/products/socs/esp32-s3/resources](https://www.espressif.com/en/products/socs/esp32-s3/resources)
- **Matter `chip-tool` Documentation:** [https://github.com/project-chip/connectedhomeip/blob/master/examples/chip-tool/README.md](https://github.com/project-chip/connectedhomeip/blob/master/examples/chip-tool/README.md) (Note: A version of this is also available locally in your `esp-matter/connectedhomeip/examples/chip-tool/` directory).

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
   - ESP-IDF v5.4.1 (Ensure environment variables are set, see `SETUP.MD`)
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
   # IMPORTANT: Ensure the ESP-IDF and ESP-Matter environments are sourced in your terminal first!
   # (e.g., run `source $IDF_PATH/export.sh` and `source $ESP_MATTER_PATH/export.sh`)
   # If you followed SETUP.MD, these paths will likely be:
   source ~/esp/esp-idf/export.sh
   source ~/esp/esp-matter/export.sh
   #
   # The target should already be set via ../SETUP.MD, but can be confirmed/set:
   idf.py set-target esp32s3
   # Configure project-specific settings (like Wi-Fi, Matter settings, GPIOs)
   idf.py menuconfig
   ```
   *For guidance on specific `menuconfig` options, use the search (`/`) and help features within `menuconfig` itself, or search for the Kconfig option name in the [ESP-IDF](https://docs.espressif.com/projects/esp-idf/en/v5.4.1/esp32s3/) and [ESP-Matter](https://docs.espressif.com/projects/esp-matter/en/latest/esp32/developing.html) documentation.*

4. Build and Flash
   ```bash
   # Build the firmware
   idf.py build
   
   # Check which port your ESP32 is connected to
   ls /dev/tty.*
   
   # Flash the firmware (replace PORT with your actual device port, e.g., /dev/tty.usbmodem101)
   # If your device is not showing up, try:
   # - Disconnecting and reconnecting the USB cable
   # - Using the other USB-C port on the ESP32 (one port is for power only)
   # - Pressing the BOOT button while connecting the device
   # - Pressing RESET button
   idf.py -p PORT flash monitor
   ```

5. Initial Setup & Commissioning

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

## Matter Development Certificates and Factory NVS Partition Generation

For detailed instructions on generating Matter development certificates and the factory NVS partition binary, please refer to **Section 6: "Generate Matter Development Certificates"** and subsequent related sections in the `SETUP.MD` file. This ensures you are following the most up-to-date and comprehensive procedure for your specific environment.

`SETUP.MD` covers:
- Generating PAA, PAI, and DAC certificates using `chip-cert`.
- Generating the Certification Declaration (CD).
- Installing and using `esp-matter-mfg-tool` to create the factory NVS partition binary.
- Identifying the NVS partition offset from `partitions.csv`.
- Instructions for flashing the `mfg_nvs.bin`.

Keeping these detailed instructions centralized in `SETUP.MD` helps avoid duplication and ensures consistency.

## Temporary Notes & Critical Learnings

This section collects important, non-obvious findings during development. This information will be integrated into the main documentation later.

### Matter Commissioning Codes (QR Code & Manual Setup)

A critical aspect of getting an ESP32-Matter device to commission with a controller (like Apple Home) is using the **correct and current commissioning codes**.

1.  **Dynamic vs. Fixed Codes:**
    *   **Default Behavior (Most Examples):** When flashing a standard ESP-Matter example (like `light` or `sensor`) without a specially prepared "factory partition" containing fixed credentials, the device will often use default test parameters or generate new ones upon first boot or after a full re-flash (e.g., after `idf.py fullclean`).
    *   **Factory Partition (Advanced):** For production or consistent testing, a factory partition can be created (e.g., using `mfg_tool.py`) and flashed to the device. This embeds fixed commissioning parameters (VID, PID, passcode, discriminator), and the QR code/manual code associated with these parameters would remain persistent across application flashes.

2.  **Finding the Correct Codes:**
    *   **Serial Monitor is Key:** The primary source for current commissioning information is the **serial monitor output** immediately after flashing and booting the device.
    *   **Explicit QR/Manual Code Logs:** Look for lines similar to:
        ```
        I (xxxx) esp_matter_qrcode: Manual pairing code: XXXXXXXXXXX
        I (xxxx) esp_matter_qrcode: Onboarding QRCode: MT:YYYYYYYYYYYYYYY
        ```
        If these are present, use them. The `MT:` string is the payload for QR code generation.
    *   **Commission Parameter Log:** If the explicit QR code logs are *not* present (as was the case with the `light` example on ESP32-S3 during recent tests), look for a line like:
        ```
        I (xxxx) chip[DIS]: Advertise commission parameter vendorID=AAAAA productID=BBBBB discriminator=CCCC/DD ...
        ```
        This provides:
        *   `vendorID` (VID)
        *   `productID` (PID)
        *   `discriminator` (Long discriminator is `CCCC`, short is often derived or part of the `DD`)
    *   **Default Test Passcode:** In the absence of an explicitly logged manual code, and if the device is using default test configurations (often true for examples), the **standard Matter test passcode `20202021`** should be attempted for manual pairing.
        *   **Experience:** For the `light` example on an ESP32-S3, when explicit QR/manual code logs were missing, using the manual setup code `20202021` with Apple Home worked successfully. The device advertised `vendorID=65521`, `productID=32768`, and `discriminator=3840`.

3.  **CRITICAL - Do NOT Reuse Old Codes:**
    *   Using a QR code or manual setup code from a *previous example build*, a *different project*, or a *generic SDK setup document* is highly likely to **FAIL**.
    *   The inability to commission the `sensor` example in earlier attempts (approx. 50 tries) was very likely due to repeatedly using a stale QR code that was no longer valid for the flashed firmware. Each significant re-flash (especially after a `fullclean`) can reset these dynamic credentials.

**Always ensure you are using the commissioning information that corresponds to the exact firmware binary currently running on the device.**

### Controller Behavior Observations (Apple Home)

During testing with Apple Home, some notable behaviors were observed:

1.  **Intermittent "No Code Needed" Commissioning:**
    *   **Observation:** On some occasions, particularly when re-adding a device that had been recently commissioned (even with different firmware like the `light` example followed by the `sensor` example), Apple Home did not prompt for a commissioning code (QR or manual). It allowed adding the device directly.
    *   **Likely Cause:** This is suspected to be due to controller-side caching (by the Home Hub or iPhone) of the device's network identifiers (e.g., MAC address). Even with new firmware, if the device appears on the network with some familiar identifiers, the controller might streamline the re-addition process or use previously exchanged security material if a fabric wasn't completely torn down on the controller side. This behavior was observed even when Wi-Fi NVS was disabled on the ESP32 (which should clear Wi-Fi credentials).

2.  **Initial Incorrect Device Type Advertisement:**
    *   **Observation:** When initiating the pairing process in Apple Home, the ESP32 device (running the `sensors` example) initially advertised itself as a "Light". However, upon proceeding with the addition, Apple Home correctly identified it as a sensor device and prompted for temperature, humidity, and occupancy settings.
    *   **Likely Cause:** This is also attributed to caching within the Apple Home app or Home Hub. The app might initially display a device based on stale mDNS (Bonjour) advertisement data or a cached "friendly name" associated with the device's network presence from a previous configuration (like the `