# Matter Generic Switch - Circuit Diagram

## ESP32-C3 SuperMini with Momentary Push-Button and Status LED

### Components
- ESP32-C3 SuperMini Board
- Momentary Push-Button (normally-open)
- 5mm LED (Red, Green, or Blue)
- 220Ω Resistor
- Dupont Wires/Connectors
- Optional: Non-conductive hot glue for stability

### Wiring Diagram (ASCII)

```
                                       +---------------------+
                                       |                     |
                                       |    Momentary        |
                                       |    Push-Button      |
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

1. **Momentary Push-Button Connections:**
   - **VCC**: Connect to ESP32-C3 SuperMini 3.3 V (or 5 V) pin
   - **Signal**: Connect to ESP32-C3 SuperMini **GPIO 3**
   - **GND**: Connect to ESP32-C3 SuperMini GND pin

2. **LED Connections:**
   - **Anode (+, longer leg)**: Connect through 220Ω resistor to ESP32-C3 SuperMini GPIO5 pin
   - **Cathode (-, shorter leg)**: Connect to ESP32-C3 SuperMini GND pin

### Notes:
- **Power Supply**: The system is powered via the ESP32-C3 SuperMini's USB-C port
- **Switch**: The push-button is read by GPIO 3 with internal pull-up enabled and debounced in software.
- **Signal Logic**: GPIO 3 reads LOW when the button is pressed (active-low) and HIGH when released.
- **LED Status**:
  - Dim/Low brightness: System powered and ready (no motion detected)
  - Bright/Blinking: Motion detected

### Assembly Tips:
1. Use Dupont connectors for all connections for easy assembly/disassembly
2. Consider adding a small dab of non-conductive hot glue at connector bases for stability
3. Ensure the push-button is accessible from outside the enclosure.
4. The LED should be visible from outside the enclosure to provide status feedback

### Enclosure Considerations:
- Old HDD enclosure provides good protection
- Ensure a hole or clear window for the push-button
- Small hole or light pipe for the status LED
- Cable management for unused ESP32 pins (they can remain exposed inside the enclosure) 