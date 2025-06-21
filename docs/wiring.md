# Wiring Guide for Matter Generic Switch

## ESP32-C3 SuperMini Wiring

### Push-Button Connections

```
ESP32-C3 SuperMini          Momentary Push-Button
+------------+              +----------+
|            |              |          |
| GND -------+--------------+ GND      |
|            |              |          |
| 3.3V ------+--------------+ VCC      |
|            |              |          |
| GPIO3 -----+--------------+ SIGNAL   |
|            |              |          |
+------------+              +----------+

*Use 3.3 V unless your button requires an external pull-up to 5 V.*
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

1. Use Dupont connectors directly to ESP32 pins for the push-button and LED.
2. Make a GND pigtail to the ESP32 GND because two ground connections are needed (button and LED).
3. Add a small dab of non-conductive hot glue at connector bases for stability.
4. Ensure the push-button is accessible from outside the enclosure.
5. Ensure the ESP32 board is protected from dust and moisture. 