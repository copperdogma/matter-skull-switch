# Matter Skull Switch - Circuit Diagram

## ESP32-C3 SuperMini with GPIO Output Signal

### Components
- ESP32-C3 SuperMini Board
- Animatronic Controller (External ESP32 or similar)

- JST-XH 3-pin Connector or Dupont Wires
- Optional: Non-conductive hot glue for cable strain relief

### Wiring Diagram (ASCII)

```
          ESP32-C3 SuperMini                    Animatronic Controller
         +-------------------------+            +----------------------+
         |                         |            |                      |
         |                     5V -+     NC     |                      |
         |                         |            |                      |
         |                         |   SIGNAL   |                      |
         |                   GPIO4 -+------+----+--> GPIO_IN (e.g. GPIO3)
         |                         |      |    |                      |
         |      SKULL SWITCH       |      |    |    SKULL AUDIO       |
         |      (Matter Device)    |      |    |    CONTROLLER        |
         |                         |      |    |                      |
         |                    GND -+------+----+--> GND               |
         |                         |           |                      |
         |                         |           +----------------------+
         |                         |                        
         |                         |                        
         |                         |                        
         |                         |                        
         |                         |                        
         |                         |                        
         |                         |                                   
         |                   USB-C |  <-- 5V Power Input              
         +-------------------------+                                   

         Signal Levels:
         GPIO4 HIGH (3.3V) = "GO!" signal
GPIO4 LOW  (0V)    = Stop/Reset signal
```

### Signal Connection Options

#### Option 1: JST-XH Connector (Recommended)
```
ESP32-C3 Pin    JST-XH Wire Color    Animatronic Controller
GPIO 4       -> Red (Signal)      -> GPIO Input Pin
GND          -> Black (Ground)    -> GND
3.3V         -> White (Power)     -> VCC (if needed)
```

#### Option 2: Dupont Connectors
```
ESP32-C3 Pin    Dupont Wire       Animatronic Controller
GPIO 4       -> Signal Wire   -> GPIO Input Pin
GND          -> Ground Wire   -> GND
```

### Pin Connections

1. **Signal Output (GPIO 4):**
   - **Function**: "GO!" signal to animatronic controller
   - **Logic Level**: 3.3V HIGH = trigger, 0V LOW = stop/reset
   - **Signal Type**: Digital output, configurable pulse or toggle mode
   - **Connection**: Direct to animatronic controller GPIO input



3. **Power Connections:**
   - **USB-C**: 5V power input to ESP32-C3 SuperMini
   - **3.3V Output**: Can power low-current animatronic circuits if needed
   - **GND**: Shared ground between devices

### Signal Timing Diagram

```
Matter "ON" Command:
    App Tap -> WiFi -> ESP32-C3 -> GPIO4
    |          |       |          |
    0ms       50ms    100ms     100ms
                               
GPIO4 Signal:
    ____                    ____
        |                  |
        |   500ms pulse    |
        |__________________|
        
Animatronic Response:
                           ____________________
                          |                    |
                          | Random Audio Play  |
                          |____________________|
```

### Notes:
- **Power Supply**: System powered via ESP32-C3 SuperMini's USB-C port
- **Signal Logic**: GPIO 4 outputs HIGH (3.3V) for "GO!" signal, LOW (0V) for stop/reset
- **Pulse Mode**: Default 500ms pulse when Matter switch turned "ON"
- **Toggle Mode**: GPIO follows Matter switch state (ON = HIGH, OFF = LOW)
- **BOOT Button**: Built-in on GPIO 9, used for factory reset (hold 5+ seconds)
- **Signal Range**: Up to 30cm cable length inside skull enclosure
- **Protection**: Consider pull-down resistor on animatronic side for noise immunity

### Assembly Tips:
1. Use JST-XH connectors for reliable connections inside skull enclosures
2. Add strain relief with heat shrink tubing or cable boots
3. Route signal cable away from power cables to minimize interference
4. Test signal levels with multimeter before connecting to animatronic controller
5. Consider optocoupler isolation for noisy environments

### Enclosure Integration:
- Compact design fits inside most animatronic skull enclosures
- USB-C port accessible for charging and programming

- Secure mounting with 3M Dual Lock fasteners
- Cable management with zip ties and adhesive anchors

### Multiple Skull Setup:
Each skull switch requires:
- Unique Matter device certificates and identifiers
- Independent GPIO output connection to its animatronic controller
- Separate commissioning in Matter controller app
- Individual or grouped control via Matter scenes 