# ESP32 Matter Occupancy Sensor Design

20250116: Created by Cam Marsollier with Claude Sonnet 3.5
20250117: Updated by Cam Marsollier with Claude Sonnet 3.5 to document verified PIR sensor capabilities
20250117: Updated by Cam Marsollier with Claude Sonnet 3.5 to simplify power management design
20250117: Updated by Cam Marsollier with Claude Sonnet 3.5 to document Matter cluster implementation

## Overview
This document details the technical implementation of the ESP32 Matter Occupancy Sensor, following the requirements specified in `spec.md`.

## Architecture
### Hardware Components
- ESP32-S3 WROOM module
  - Dual-core 240 MHz processor
  - 512KB SRAM for Matter stack
  - 8MB Flash for firmware/storage
  - Dual USB-C ports
  - 5V and 3.3V power output capabilities
- HC-SR501 PIR sensor
  - Maximum sensitivity for range
  - ~120° detection angle
  - Optimized mounting for coverage
  - Powered directly from ESP32's 5V pin
  - Initial Test Results (20250117):
    - Configuration:
      - Sensitivity: Medium (potentiometer at middle position)
      - Time delay: Minimum (fully counter-clockwise)
      - Trigger mode: 'H' for repeatable triggers
    - Signal Characteristics:
      - Baseline: 0V (no motion)
      - Trigger: Clean 3.3V spike on motion detection
      - Duration: 1-2 seconds per trigger
      - Block time: ~2.5s between triggers (built-in lockout period)
    - Validation:
      - Output voltage compatible with ESP32 GPIO (3.3V)
      - Clean signal transitions with minimal noise
      - Consistent triggering behavior
      - Built-in debouncing via block time
- Power Management
  - USB-C power input (5V)
  - Direct 5V to PIR sensor
  - 3.3V regulated for ESP32 core and GPIO

### Software Stack
- ESP-IDF v5.1+
- Matter Protocol Integration
  - Occupancy sensor cluster (0x0406)
  - Standard commissioning flow
  - SPAKE2+ security
- Motion Detection
  - 50ms debouncing logic
  - 5-minute occupancy timeout
  - False positive mitigation

## Implementation Phases

### Phase 1: Hardware Setup
1. Component Assembly
   - Prototype board layout
   - Power distribution
   - Sensor integration
   - LED indicators

2. Power System
   - USB-C PD implementation
   - Voltage regulation
   - Power stability testing

### Phase 2: Core Firmware
1. Development Environment
   - ESP-IDF setup
   - Matter SDK configuration
   - Debug infrastructure

2. Sensor Implementation
   - GPIO configuration
   - Interrupt handling
   - State management
   - Motion processing

### Phase 3: Matter Integration
1. Protocol Stack
   - Core configuration
   - Security setup
   - Network handling

2. Device Integration
   - Cluster implementation
   - Attribute management
   - Commissioning flow

### Phase 4: Testing & Validation
1. Environmental Testing
   - Light interference
   - EMI mitigation

2. Integration Testing
   - Range verification
   - HomeKit compatibility
   - Power stability

## Technical References
### ESP32 Development
- [Freenove ESP32-S3 WROOM Board Documentation](https://github.com/Freenove/Freenove_ESP32_S3_WROOM_Board/tree/main/Datasheet)
  - Official manufacturer repository
  - Contains pinout diagrams
  - Technical specifications
  - Last accessed: 20250117
- [ESP32 Motion Sensor Tutorial](https://esp32io.com/tutorials/esp32-motion-sensor)
  - HC-SR501 implementation
  - GPIO configuration
  - Timing parameters

### Matter Protocol
- [ESP Matter Solution](https://www.espressif.com/en/solutions/device-connectivity/esp-matter-solution)
  - Platform requirements
  - Development setup
  - Security implementation

- [Matter Development Guide](https://developer.espressif.com/blog/matter/)
  - Cluster specifications
  - Commissioning flow
  - QR code implementation

### Additional Documentation
- [ESP-IDF Programming Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/index.html)
- [Matter Specification](https://csa-iot.org/developer-resource/specifications-download-request/) 

### HC-SR501 PIR Sensor Specifications
- Operating Voltage: DC 4.5V-20V
- Power Consumption: <65μA
- Output: Digital pulse high (3.3V) / low (0V)
- Trigger Mode: L=non-repeatable, H=repeatable
- Delay Time: Adjustable 0.3s to 5min
- Block Time: 2.5s (default)
- Detection Range: 3-7m adjustable, up to 120° angle
- Temperature Range: -15°C to +70°C
- Dimensions: 32×24mm
- Note: This sensor uses a pyroelectric sensor for motion detection only, no temperature measurement capability
- Reference: [HC-SR501 Datasheet](https://www.mpja.com/download/31227sc.pdf) 

# Design Decisions

20250116: Created by Cam Marsollier and Claude Sonnet 3.5
20250117: Updated by Cam Marsollier with Claude Sonnet 3.5 to document occupancy delay research
20250117: Updated by Cam Marsollier with Claude Sonnet 3.5 to document power management simplification
20250117: Updated by Cam Marsollier with Claude Sonnet 3.5 to document Matter cluster implementation

## Matter Cluster Implementation

### Research Findings
- Cluster ID: OccupancySensing (0x0406)
- Relevant Attributes:
  - PIROccupiedToUnoccupiedDelay: Controls timeout period
  - PIRUnoccupiedToOccupiedDelay: For immediate triggering
  - OccupancySensorType: Set to PIR type
  - OccupancySensorTypeBitmap: Set to PIR capability

### Design Decision
Selected Implementation:
- Use standard Matter OccupancySensing cluster
- Configure as PIR sensor type
- Leverage built-in delay attributes for configuration
- Implement persistence using Matter attribute storage

### Rationale
1. Standard Compliance:
   - Uses official Matter cluster (0x0406)
   - Follows Matter-defined attribute patterns
   - Ensures compatibility with Matter controllers

2. Delay Configuration:
   - PIROccupiedToUnoccupiedDelay for 5-30 minute range
   - PIRUnoccupiedToOccupiedDelay set to 0 for instant triggers
   - Values persist through Matter's attribute storage

3. Sensor Type Configuration:
   - Explicitly identify as PIR sensor
   - Proper capability reporting to controllers
   - Enables type-specific behavior in Home app

### References
- ESP Matter Repository (commit TBD): https://github.com/espressif/esp-matter
  - Last accessed: 20250117
  - Key examples: sensors/main/app_main.cpp
  - Key components: esp_matter_cluster.h, esp_matter_attribute.h
- Matter Specification: OccupancySensing Cluster
  - Version: TBD
  - Key sections: Cluster ID 0x0406, PIR attributes

## Occupancy Delay Configuration

### Research Findings
Based on industry research and standards:
- Traditional delay ranges: 5-30 minutes
- Energy code trends: Moving from 30 to 20 minute maximums
- LED considerations: Can support shorter delays (1-5 minutes) due to reduced impact on fixture life
- Application-specific recommendations:
  - Restrooms: 20-30 minutes (prevent lights going out while occupied)
  - Private offices: 15-20 minutes
  - Corridors: 5-10 minutes
  - Warehouses: 20-30 minutes (especially near equipment)

### Design Decision
Selected Configuration:
- Default delay: 15 minutes
- Configurable range: 5-30 minutes
- Implementation: Matter-configurable attribute
- User control: Exposed in Home app settings

### Rationale
1. Default of 15 minutes:
   - Balances energy savings with reliable detection
   - Common industry standard for mixed-use spaces
   - Sufficient time to detect subtle movements
   - Reduces false negatives in garage environment

2. Configurable range:
   - Minimum 5 minutes: Suitable for LED lighting
   - Maximum 30 minutes: Compliant with energy codes
   - User adjustable: Accommodates different use patterns

3. Matter integration:
   - Implement as configurable cluster attribute
   - Persist settings across power cycles
   - Update occupancy reporting based on current delay setting

### References
- Lighting Controls Association guidelines
- Industry forum discussions
- Energy code requirements 

# Power Management

20250117: Created by Cam Marsollier with Claude Sonnet 3.5 to document power analysis and design decisions

## Requirements Analysis
### HC-SR501 PIR Requirements:
- Operating Voltage: DC 4.5V-20V
- Power Consumption: <65μA
- Digital Output: 3.3V/0V logic levels

### ESP32-S3 Power Specifications:
- USB-C input: 5V @ 500mA maximum (2.5W)
- Core Components:
  - CPU at full load: ~200mA
  - Wi-Fi active: ~100-120mA
  - Matter stack overhead: Minimal beyond Wi-Fi usage
  Total ESP32 consumption: ~320mA

## Power Supply Considerations
- USB-C input: 5V @ 500mA maximum (2.5W)

## Design Decision
Selected Configuration:
- Direct 5V power from ESP32 to PIR sensor
- Remove separate USB-C PD trigger board
- Utilize ESP32's onboard power management

### Rationale
1. Simplified Hardware:
   - Reduced component count
   - Lower assembly complexity
   - Decreased points of failure
   - Cost reduction

2. Power Requirements:
   - System well within USB-C power specifications
   - Adequate headroom for power fluctuations
   - No additional regulation needed
   - PIR's draw well within ESP32's capacity

3. Reliability:
   - Direct power connection reduces noise
   - USB power typically very stable
   - Fewer components to fail

## Conclusions
- System is well within USB-C power specifications
- Adequate headroom exists for power fluctuations
- No additional power supply or regulation needed
- PIR sensor can be powered directly from ESP32's 5V output
- Matter stack adds minimal overhead to existing Wi-Fi power usage

## References
- HC-SR501 Datasheet (v1.2): https://www.mpja.com/download/31227sc.pdf
  - Last accessed: 20250117
  - Key specs: Operating voltage, power consumption, output levels
- ESP32-S3 Technical Reference Manual: https://www.espressif.com/sites/default/files/documentation/esp32-s3_technical_reference_manual_en.pdf
  - Version: 1.1
  - Last accessed: 20250117
  - Key sections: Power Management (Chapter 5)
- ESP32 Hardware Design Guidelines: https://www.espressif.com/sites/default/files/documentation/esp32_hardware_design_guidelines_en.pdf
  - Version: 3.0
  - Last accessed: 20250117
  - Key sections: Power Supply Design

## Project Structure

The project is organized into two main components, each with its own Cursor workspace but managed in a single GitHub repository:

```
esp32-matter-occupancy/              # Root directory (GitHub repository)
├── docs/                           # Documentation workspace
│   ├── spec.md                    # Project specifications
│   ├── design.md                  # Technical design decisions
│   ├── todo.md                    # Task tracking
│   ├── CHANGELOG.md               # Project history
│   ├── references/                # Reference materials (git-ignored)
│   │   ├── datasheets/           # Component datasheets
│   │   ├── specs/                # Downloaded specifications
│   │   └── research/             # Research materials
│   └── assets/                    # Project assets (diagrams, images)
│
├── firmware/                      # Firmware workspace
│   ├── src/                      # Source code
│   ├── include/                  # Header files
│   ├── components/               # ESP-IDF components
│   ├── main/                     # Main application code
│   └── sdkconfig                 # ESP-IDF configuration
│
├── .gitignore                    # Git ignore rules
└── README.md                     # Project overview
```

### Key Considerations:
- Documentation and firmware are separate Cursor workspaces but part of the same Git repository
- The `references/` directory is git-ignored and used for downloaded specifications, research materials, and reference implementations
- Each workspace has its own `.cursorrules` file for specialized Cursor configuration
- Common assets like diagrams and images are stored in `docs/assets/` and version controlled 