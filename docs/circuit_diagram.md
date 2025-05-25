# Matter Occupancy Sensor - Circuit Diagram

## ESP32-C3 SuperMini with HC-SR501 PIR Sensor and Status LED

### Components
- ESP32-C3 SuperMini Board
- HC-SR501 PIR Motion Sensor
- 5mm LED (Red, Green, or Blue)
- 220Ω Resistor
- Dupont Wires/Connectors
- Optional: Non-conductive hot glue for stability

### Wiring Diagram (ASCII)

```
                                       +---------------------+
                                       |                     |
                                       |    HC-SR501 PIR     |
                                       |                     |
                                       |  +----+----+----+   |
                                       |  |    |    |    |   |
                                       |  |    |    |    |   |
                                       |  +----+----+----+   |
                                       |    |    |    |      |
                                       +----+----+----+------+
                                            |    |    |       
                                            |    |    |       
                                          VCC  OUT  GND      
                                            |    |    |       
                                            |    |    |       
          +-------------------------+       |    |    |       
          |                         |       |    |    |       
          |                     5V -+-------+    |    |       
          |                         |            |    |       
          |                   GPIO3 -+------------+   |       
          |                         |                |       
          |      ESP32-C3           |                |       
          |      SuperMini          |                |       
          |                         |                |       
          |                         |                |       
          |                    GND -+----------------+       
          |                         |                        
          |                         |                 LED    
          |                   GPIO5 -+-------+      +----->  
          |                         |       |      |         
          |                         |      220Ω    |         
          |                         |       |      |         
          |                    GND -+---------------+         
          |                         |                        
          +-------------------------+                        
```

### Pin Connections

1. **HC-SR501 PIR Sensor Connections:**
   - **VCC**: Connect to ESP32-C3 SuperMini 5V pin
   - **OUT**: Connect to ESP32-C3 SuperMini GPIO3 pin
   - **GND**: Connect to ESP32-C3 SuperMini GND pin

2. **LED Connections:**
   - **Anode (+, longer leg)**: Connect through 220Ω resistor to ESP32-C3 SuperMini GPIO5 pin
   - **Cathode (-, shorter leg)**: Connect to ESP32-C3 SuperMini GND pin

### Notes:
- **Power Supply**: The system is powered via the ESP32-C3 SuperMini's USB-C port
- **PIR Sensor**: The HC-SR501 has adjustable sensitivity and delay settings (via on-board potentiometers)
- **Signal Logic**: The PIR sensor outputs HIGH (3.3V) when motion is detected and LOW (0V) when idle
- **LED Status**:
  - Dim/Low brightness: System powered and ready (no motion detected)
  - Bright/Blinking: Motion detected

### Assembly Tips:
1. Use Dupont connectors for all connections for easy assembly/disassembly
2. Consider adding a small dab of non-conductive hot glue at connector bases for stability
3. When mounting in enclosure, ensure the PIR sensor lens has clear line of sight
4. The LED should be visible from outside the enclosure to provide status feedback

### Enclosure Considerations:
- Old HDD enclosure provides good protection
- Ensure a hole or clear window for the PIR sensor lens
- Small hole or light pipe for the status LED
- Cable management for unused ESP32 pins (they can remain exposed inside the enclosure) 