# Animatronic Skull Integration Guide

## Overview
This guide details how to integrate the ESP32-C3 SuperMini Matter Skull Switch with animatronic skulls and other Halloween effects. The skull switch sends a GPIO "GO!" signal to trigger audio playback and animations.

## System Architecture

```
Smart Home App -> Matter Network -> Skull Switch -> GPIO Signal -> Animatronic Controller -> Audio/Servo Output
     (Apple Home)      (WiFi)       (ESP32-C3)      (3.3V)          (ESP32/Audio)       (Speaker/Motors)
```

## Hardware Integration

### Signal Connection Methods

#### Method 1: Direct GPIO Connection (Recommended)
```
Skull Switch (ESP32-C3)      Animatronic Controller (ESP32)
+-------------------+        +------------------------+
| GPIO3 (Signal) ---+--------+---> GPIO4 (Input)     |
| GND            ---+--------+---> GND               |
| 3.3V (optional) --+--------+---> VCC (if needed)   |
+-------------------+        +------------------------+
```

#### Method 2: Optocoupler Isolation (Noisy Environments)
```
Skull Switch                 Optocoupler              Animatronic Controller
+------------+              +------------+            +------------------+
| GPIO3 -----|--[220Ω]-----|---> LED    |            |                  |
|            |              |            |     +------|---> GPIO4        |
| GND -------|--------------|---> GND    |     |      |                  |
+------------+              |            |  Collector |                  |
                            | Emitter ---+-----+      |                  |
                            +------------+            |                  |
                                                      | GND              |
                                                      +------------------+
```

### Power Supply Options

#### Option 1: Separate Power (Current Implementation)
- Skull Switch: USB-C 5V power
- Animatronic Controller: Separate power supply
- Only signal and ground connections between devices

#### Option 2: Shared Power (Future Enhancement)
- LiPo battery with TP4056 USB-C charging
- 3.3V LDO regulator supplies both devices
- Power Source cluster reports battery level to Matter

### Physical Installation

#### Inside Skull Placement
```
                    Skull Top View
          +---------------------------+
          |                           |
          |  [Animatronic Controller] |
          |  (ESP32 + Audio Module)   |
          |                           |
          |     [Skull Switch]        |
          |     (ESP32-C3)           |
          |                           |
          |  [Speaker]    [Servo]     |
          |                           |
          +---------------------------+
                    USB-C Access
```

#### Mounting Recommendations
1. **Skull Switch**: 3M Dual Lock fasteners on flat surface
2. **Cable Management**: Zip ties and adhesive anchors
3. **USB Access**: Route USB-C port near skull opening
4. **Signal Routing**: Keep signal wires away from power cables
5. **Heat Management**: Ensure airflow around electronics

## Software Integration

### Animatronic Controller Code (ESP32)
```cpp
// Basic GPIO trigger detection
const int TRIGGER_PIN = 4;  // GPIO4 input from skull switch
const int SPEAKER_PIN = 25; // Audio output
bool triggered = false;

void setup() {
    pinMode(TRIGGER_PIN, INPUT_PULLDOWN);
    // Setup audio system
    attachInterrupt(digitalPinToInterrupt(TRIGGER_PIN), onTrigger, RISING);
}

void onTrigger() {
    triggered = true;
}

void loop() {
    if (triggered) {
        triggered = false;
        playRandomAudio();
        // Trigger servos, lights, etc.
    }
}
```

### Skull Switch Configuration (ESP32-C3)
```cpp
// GPIO output control
const int SIGNAL_PIN = 3;   // GPIO3 output to animatronic
const int PULSE_DURATION = 500; // 500ms pulse

void onMatterSwitchOn() {
    digitalWrite(SIGNAL_PIN, HIGH);
    delay(PULSE_DURATION);
    digitalWrite(SIGNAL_PIN, LOW);
}

void onMatterSwitchOff() {
    digitalWrite(SIGNAL_PIN, LOW);
}
```

## Signal Timing and Protocols

### Pulse Mode (Default)
```
Matter "ON" Command:
GPIO3: _____/‾‾‾‾‾\_____
       0ms  100ms 600ms

Animatronic Response:
Action: _______/‾‾‾‾‾‾‾‾‾‾‾‾\_____
        0ms    200ms      5000ms
               (Random audio plays)
```

### Toggle Mode (Optional)
```
Matter "ON":  GPIO3 goes HIGH and stays HIGH
Matter "OFF": GPIO3 goes LOW

Use for continuous effects or state-based animations
```

### Edge Detection
```cpp
// Recommended interrupt handler on animatronic side
volatile bool triggerFlag = false;

void IRAM_ATTR triggerISR() {
    static unsigned long lastTrigger = 0;
    unsigned long now = millis();
    
    // Debounce: ignore triggers within 100ms
    if (now - lastTrigger > 100) {
        triggerFlag = true;
        lastTrigger = now;
    }
}
```

## Multiple Skull Coordination

### Independent Operation
Each skull switch operates independently:
- Unique Matter device certificates
- Separate GPIO connections to respective animatronic controllers
- Individual commissioning in Matter apps

### Synchronized Effects
Use Matter scenes for coordinated animations:
1. Create scene in Apple Home: "Halloween Scare"
2. Add multiple skull switches to scene
3. Single scene activation triggers all skulls simultaneously
4. Network timing introduces natural variation (50-200ms spread)

### Sequential Triggering
```cpp
// Example: Trigger skulls in sequence with delays
void triggerSequence() {
    skullSwitch1.turnOn();
    delay(1000);
    skullSwitch2.turnOn();
    delay(1000);
    skullSwitch3.turnOn();
}
```

## Power Management

### Battery Life Estimation (Future)
```
Components:
- ESP32-C3: ~50mA active, ~10µA deep sleep
- Status LED: ~5mA when on
- GPIO output: ~1mA when HIGH

1000mAh Battery Life:
- Continuous: ~18 hours
- With sleep (trigger once/hour): ~2-3 months
- With Power Source cluster reporting every 10 minutes
```

### Charging Strategy
- TP4056 module with USB-C input
- 4.2V LiPo battery
- 3.3V LDO regulation
- Battery voltage monitoring via ADC

## Troubleshooting

### Signal Issues
| Problem | Symptoms | Solution |
|---------|----------|----------|
| No trigger response | GPIO stays LOW | Check wiring, test with multimeter |
| False triggers | Random activations | Add pull-down resistor, check for EMI |
| Weak signal | Intermittent triggers | Verify 3.3V signal level, check connections |
| Delayed response | Slow activation | Check WiFi latency, Matter network performance |

### Power Issues
| Problem | Symptoms | Solution |
|---------|----------|----------|
| Device won't start | No LED, no network | Check USB-C power, verify 5V supply |
| Frequent resets | Unstable operation | Check power consumption, add capacitors |
| Battery drains fast | Short runtime | Implement sleep modes, reduce LED usage |

### Matter Integration Issues
| Problem | Symptoms | Solution |
|---------|----------|----------|
| Won't commission | Setup fails | Factory reset, check QR code |
| Disconnects often | Unreliable control | Check WiFi signal strength, router settings |
| Slow response | Delayed activation | Optimize WiFi, check network congestion |

## Advanced Features (Future)

### Pattern Generation
```cpp
// Multiple trigger patterns
enum TriggerPattern {
    SINGLE_PULSE,    // 500ms pulse
    DOUBLE_PULSE,    // Two 200ms pulses with 100ms gap
    LONG_PULSE,      // 2000ms pulse
    MORSE_CODE       // Custom pattern
};
```

### Battery Monitoring
```cpp
// Report battery level via Matter Power Source cluster
uint8_t getBatteryPercentage() {
    int adcValue = analogRead(BATTERY_ADC_PIN);
    float voltage = (adcValue * 3.3 * 2.0) / 4095.0; // Voltage divider
    return map(constrain(voltage * 100, 320, 420), 320, 420, 0, 100);
}
```

### Scene Integration
- Scene cluster implementation for complex trigger patterns
- Multiple endpoints for different effect types
- Integration with other Halloween decorations

## Safety Considerations

1. **Electrical Safety**
   - Use proper connectors (JST-XH recommended)
   - Ensure proper grounding between devices
   - Protect against reverse polarity

2. **Heat Management**
   - Ensure adequate ventilation in skull enclosures
   - Monitor component temperatures during extended use
   - Consider thermal shutdown for safety

3. **EMI Protection**
   - Use shielded cables in noisy environments
   - Add ferrite cores if experiencing interference
   - Keep signal wires away from motor/servo power cables

## Testing and Validation

### Signal Testing
```bash
# Using multimeter on GPIO3 pin
# Should read 0V normally, 3.3V during pulse
```

### Matter Testing
```bash
# Commission device in Apple Home
# Test on/off commands
# Verify response timing
# Test scene integration
```

### Integration Testing
```bash
# End-to-end test:
# App tap -> Matter command -> GPIO signal -> Audio playback
# Measure total latency (target: <1 second)
```

This integration guide provides the foundation for connecting Matter skull switches to various animatronic controllers and Halloween effects systems. 