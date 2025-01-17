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

## Development Setup

1. Install Prerequisites
   - ESP-IDF v5.1+
   - Matter SDK
   - Development certificates

2. Clone Repository
   ```bash
   git clone [repository-url]
   cd esp32-matter-occupancy
   ```

3. Build Environment
   ```bash
   cd firmware
   idf.py set-target esp32s3
   idf.py menuconfig  # Configure Matter settings
   ```

4. Build & Flash
   ```bash
   idf.py build
   idf.py -p [PORT] flash
   ```

## Project Structure

```
esp32-matter-occupancy/
├── docs/                    # Documentation
│   ├── spec.md             # Requirements
│   ├── design.md           # Technical design
│   ├── todo.md             # Task tracking
│   └── CHANGELOG.md        # Project history
│
└── firmware/               # ESP32 firmware
    ├── src/               # Source code
    ├── include/          # Header files
    ├── components/       # ESP-IDF components
    └── main/            # Main application
```

## Documentation

- [Specifications](docs/spec.md) - Detailed project requirements
- [Design Decisions](docs/design.md) - Technical implementation details
- [Changelog](docs/CHANGELOG.md) - Project history and updates
- [Todo](docs/todo.md) - Current development status

## Contributing

1. Fork the repository
2. Create a feature branch
3. Commit your changes
4. Push to the branch
5. Create a Pull Request

## License

[License Type] - See LICENSE file for details

## Author

Cam Marsollier 