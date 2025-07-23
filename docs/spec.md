# Matter Skull Switch – Technical Specification

**Created:** 2025-07-22 by Cam Marsollier  

## Core Purpose
Create a reliable, Matter-compatible On/Off Switch using ESP32-C3 SuperMini to remotely trigger animatronic skulls and Halloween effects. The device sends GPIO "GO!" signals to connected animatronic controllers when activated via Apple Home, Google Home, or Amazon Alexa.

## Fundamental Principles
1. **Reliable GPIO Control**
   - Clean 3.3V signal output to animatronic controllers
   - Configurable pulse duration for different trigger types
   - Low-latency Matter command to GPIO response

2. **Easy Integration**
   - QR code-based Matter setup
   - Standard On/Off switch behavior in smart home apps
   - Compatible with scenes and automations

3. **Robust Operation**
   - Stable operation inside skull enclosures
   - Shared power supply with animatronic systems
   - Future battery power with level reporting

## Core Requirements

### 1. Hardware Integration
- ESP32-C3 SuperMini as main controller:
  - Single-core RISC-V 160 MHz processor
  - 400KB SRAM for Matter stack
  - 4MB Flash for firmware and storage
  - Native USB support for programming and debugging
  - Built-in Wi-Fi for Matter connectivity
  - 3.3V GPIO output capability

- GPIO Signal Output:
  - GPIO 3 configured as digital output
  - 3.3V HIGH = "GO!" signal to animatronic
  - 0V LOW = stop/reset signal
  - Configurable pulse duration (50ms to 5000ms)
  - Signal routing to animatronic controller via dupont or JST connectors



- Power Management:
  - USB-C power input (5V) with 3.3V regulation
  - Shared power rail with animatronic controller (future)
  - LiPo battery support with TP4056 charging (future enhancement)

### 2. Matter Device Implementation
- Device Type: On/Off Switch (not Generic Switch)
- Required Clusters:
  - Basic Information Cluster (mandatory)
  - Identify Cluster (mandatory)
  - On/Off Cluster (core functionality)
- Optional Clusters:
  - Power Source Cluster (for battery level reporting)
  - Scenes Cluster (for multiple trigger patterns)

- Matter Attributes:
  - OnOff.OnOff: Boolean state (true = GPIO HIGH, false = GPIO LOW)
  - Basic.VendorID: 0xFFF1 (test vendor)
  - Basic.ProductID: 0x8001 (skull switch)
  - Basic.NodeLabel: "Skull Switch"

### 3. GPIO Signal Behavior
- Signal Timing:
  - **Pulse Mode (Default):** GPIO goes HIGH for configured duration, then LOW
  - **Toggle Mode (Optional):** GPIO state follows Matter switch state
  - **Pulse Duration:** 500ms default, configurable 50ms-5000ms
  - **Signal Voltage:** 3.3V ± 0.1V

- Signal Response:
  - Matter "ON" command → GPIO HIGH within 100ms
  - Matter "OFF" command → GPIO LOW within 100ms
  - Network latency: < 500ms from app tap to GPIO change
  - Hardware debouncing: 10ms minimum between state changes

### 4. Animatronic Integration
- Connection Method:
  - 3-wire connection: Signal, 3.3V (optional), GND
  - JST-XH or dupont connector for reliability
  - Signal wire length: up to 30cm inside skull enclosure

- Signal Protocol:
  - Rising edge trigger: LOW to HIGH transition starts animation
  - Signal level: 3.3V logic compatible with ESP32 inputs
  - Pull-down resistor on animatronic side recommended
  - Signal isolation: optocoupler optional for noise immunity

- Multiple Device Coordination:
  - Each skull switch operates independently
  - Matter scenes can synchronize multiple switches
  - Individual addressing via unique device certificates

### 5. Power Supply Requirements
- Current Configuration:
  - USB-C 5V input to ESP32-C3 SuperMini
  - Internal 3.3V regulation for logic and GPIO
  - Power consumption: < 500mA typical, < 1A peak

- Future Battery Configuration:
  - 3.7V LiPo battery (1000mAh+ recommended)
  - TP4056 USB-C charging module
  - 3.3V LDO regulation (AMS1117 or HT7333)
  - Battery voltage monitoring via ADC with voltage divider

### 6. Environmental Considerations
- Operating Environment:
  - Temperature: 0°C to 40°C (indoor Halloween displays)
  - Humidity: 10% to 85% non-condensing
  - Dust protection: IP20 minimum (enclosed in skull)

- Mounting and Installation:
  - 3M Dual Lock fasteners for board attachment
  - Zip ties and adhesive anchors for cable management
  - USB-C port accessible for charging/programming
  - Heat dissipation consideration in enclosed spaces

## Success Criteria
1. Reliable GPIO signal generation within 100ms of Matter command
2. Successful Matter integration with Apple Home, Google Home, Alexa
3. Stable operation on USB-C power for extended periods
4. Quick setup via QR code commissioning
5. Signal voltage within 3.3V ± 0.1V specification
6. Less than 1 false trigger per day under normal operation

## Technical Specifications

### Hardware Specifications
- **Microcontroller:** ESP32-C3 SuperMini
- **GPIO Output:** GPIO 3, 3.3V logic level

- **Factory Reset:** GPIO 9 (BOOT button)
- **Power Input:** USB-C 5V
- **Signal Connector:** JST-XH 3-pin or dupont headers

### Software Specifications
- **ESP-IDF Version:** 5.4.1 or newer
- **ESP-Matter SDK:** Latest stable release
- **Matter Version:** 1.4
- **Device Type:** On/Off Switch (0x0103)
- **Firmware Size:** < 2MB (leaves space for OTA updates)

### Performance Specifications
- **Signal Response Time:** < 100ms from Matter command
- **Network Latency:** < 500ms end-to-end from app to GPIO
- **Power Consumption:** < 500mA @ 5V typical
- **Signal Accuracy:** 3.3V ± 0.1V
- **Pulse Duration Range:** 50ms to 5000ms, configurable

## Development Priorities

### MVP Phase (Current)
1. Basic On/Off switch Matter implementation
2. GPIO 3 output control with configurable pulse duration
3. USB-C power operation
4. QR code commissioning
5. Apple Home integration testing

### Enhancement Phase (Future)
1. LiPo battery power with voltage monitoring
2. Power Source cluster implementation for battery reporting
3. Scene cluster for multiple trigger patterns
4. Multiple GPIO outputs for complex animatronics
5. Over-the-air firmware updates
6. Advanced pulse patterns and timing controls

### Production Phase (Future)
1. Custom PCB design with integrated charging
2. 3D printed enclosures for different skull types
3. Professional certification and compliance testing
4. Multi-unit manufacturing and quality control
5. User documentation and installation guides