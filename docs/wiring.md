# Wiring Guide for Matter Occupancy Sensor

## ESP32-C3 SuperMini Wiring

### PIR Sensor Connections

```
ESP32-C3 SuperMini          HC-SR501 PIR
+------------+              +----------+
|            |              |          |
| GND -------+--------------+ GND      |
|            |              |          |
| 5V* -------+--------------+ VCC      |
|            |              |          |
| GPIO3 ------+--------------+ OUT      |
|            |              |          |
+------------+              +----------+

* 3.3V can be used if your PIR sensor supports it
```

### LED Indicator

```
ESP32-C3 SuperMini          LED (with 220Ω resistor)
+------------+              +----------+
|            |              |          |
| GND -------+--------------+ Cathode(-)|
|            |              |          |
| GPIO5 ------+----[220Ω]----+ Anode(+) |
|            |              |          |
+------------+              +----------+
```

## Notes for USB-C Breakout Board

If using a USB-C breakout board with the ESP32-C3 SuperMini:

1. Connect the USB-C breakout to provide power to the ESP32-C3:
   ```
   USB-C Breakout           ESP32-C3 SuperMini
   +------------+           +-------------+
   |            |           |             |
   | VBUS (5V) --+-----------+ 5V          |
   |            |           |             |
   | GND --------+-----------+ GND         |
   |            |           |             |
   +------------+           +-------------+
   ```

2. For programming/debugging, use the ESP32-C3's built-in USB connection or connect the USB-C breakout's data lines if needed for a custom setup.

## Physical Mounting Recommendations

1. Use Dupont connectors directly to ESP32 pins for PIR sensor and LED
2. Make GND pigtail to ESP32 GND because we need two ground connections (PIR sensor and LED)
3. Add small dab of non-conductive hot glue at connector bases for stability
4. Mount PIR sensor at 7-10 ft height for optimal coverage
5. Ensure ESP32 board is protected from dust and moisture 