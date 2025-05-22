# ESP32-C3 SuperMini Migration Guide

This document provides detailed instructions for migrating the Matter Occupancy Sensor project from ESP32-S3 to ESP32-C3 SuperMini.

## Hardware Differences

| Feature | ESP32-S3 | ESP32-C3 SuperMini | Impact |
|---------|----------|-------------------|--------|
| Processor | Dual-core Xtensa 240MHz | Single-core RISC-V 160MHz | Lower performance ceiling, but sufficient for this application |
| SRAM | 512KB | 400KB | Reduced memory available, optimizations needed |
| Flash | 8MB | 4MB | Smaller program space, may need partition adjustments |
| Size | Larger | Ultra-compact (22.5x18mm) | More suitable for space-constrained installations |
| GPIOs | More available | 11 usable GPIOs | Careful pin selection needed |
| USB | Two USB-C ports | Native USB | Different flashing procedure |
| Power | 5V via USB-C | 5V via pins | May require external USB-C breakout for power |

## Pin Selection Rationale

When choosing pins for the ESP32-C3, we need to consider several constraints:

1. **Strapping Pins** - GPIO 2, 8, and 9 affect boot behavior and must maintain certain states during reset.
2. **JTAG Pins** - GPIO 4-7 are used for JTAG debugging and may have restrictions.
3. **UART Pins** - GPIO 20 (RX) and 21 (TX) are used for serial communication.
4. **Flash Memory Pins** - GPIO 11-17 are connected to the integrated SPI flash and unavailable.

Based on these constraints, here's our pin selection:

| Function | ESP32-S3 Pin | ESP32-C3 Pin | Rationale |
|----------|--------------|--------------|-----------|
| PIR Sensor | GPIO 4 | GPIO 3 | Safe GPIO with ADC support, not a strapping pin |
| LED Indicator | GPIO 6 | GPIO 10 | Safe general-purpose GPIO |
| I2C SDA (if needed) | GPIO 4 | GPIO 8 | Default I2C data line on C3 |
| I2C SCL (if needed) | GPIO 5 | GPIO 9 | Default I2C clock line on C3 |

## Migration Steps

### 1. Update the Target and Build Configuration

```bash
cd firmware
source ~/esp/esp-idf/export.sh
source ~/esp/esp-matter/export.sh
idf.py set-target esp32c3
idf.py reconfigure
```

### 2. Build and Flash

```bash
idf.py build
idf.py -p PORT flash monitor
```

**Note:** Replace PORT with your device's port (e.g., `/dev/ttyACM0` or `/dev/ttyUSB0`).

### 3. Entering Bootloader Mode

Unlike ESP32-S3, which often requires button combinations:

1. The ESP32-C3 SuperMini has a dedicated BOOT button
2. Hold the BOOT button while connecting USB or pressing RESET
3. Release the BOOT button after 1 second

## Troubleshooting

### Common Issues and Solutions

1. **Flash Too Small Error**
   - Check partition table is suitable for 4MB flash
   - Reduce application size if necessary

2. **Memory Allocation Failures**
   - Reduce MQTT/WiFi buffer sizes
   - Optimize stack sizes for tasks

3. **USB Connection Issues**
   - Ensure your system recognizes the ESP32-C3's native USB CDC
   - Try a different USB cable, preferably data-capable

4. **Bootloader Mode Problems**
   - Timing matters - try holding BOOT button slightly longer
   - Some USB hubs can cause issues, try connecting directly to computer

## Performance Optimizations

The ESP32-C3 has lower specs than the ESP32-S3, so consider these optimizations:

1. Use the correct compiler flags for the RISC-V architecture
2. Minimize stack usage in tasks
3. Take advantage of the light sleep mode for power savings
4. Keep WiFi/Bluetooth duty cycle low

## Future Considerations

1. **Enclosure**: Design a custom 3D-printable case for the compact ESP32-C3 SuperMini
2. **Power Management**: Consider implementing deep sleep modes for battery operation
3. **Size Optimization**: The C3's small form factor enables more discreet installation options

## References

1. [ESP32-C3 SuperMini Pinout](https://www.espboards.dev/esp32/esp32-c3-super-mini/)
2. [ESP32-C3 Technical Reference Manual](https://www.espressif.com/sites/default/files/documentation/esp32-c3_technical_reference_manual_en.pdf)
3. [ESP32-C3 Datasheet](https://www.espressif.com/sites/default/files/documentation/esp32-c3_datasheet_en.pdf)
4. [ESP-IDF ESP32-C3 Programming Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c3/) 