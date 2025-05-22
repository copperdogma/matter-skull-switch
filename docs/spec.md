# Matter Garage Occupancy Sensor

20250116: Created by Cam Marsollier

## Core Purpose
Create a reliable, Matter-compatible occupancy sensor for garage monitoring that seamlessly integrates with Apple Home via USB-C power and QR code provisioning.

## Fundamental Principles
1. **Reliable Detection**
   - Accurate occupancy sensing across 48'x48' space
   - Coverage focused on person-door area
   - Consistent Matter status updates

2. **Easy Integration**
   - QR code-based Matter setup
   - Simple USB-C power connection
   - Immediate Apple Home recognition

3. **Robust Operation**
   - Handle garage environmental conditions
   - Stable power supply management
   - Reliable network connection

## Core Requirements

### 1. Hardware Integration
- ESP32-S3 WROOM as main controller:
  - Dual-core 240 MHz processor
  - 512KB SRAM for Matter stack
  - 8MB Flash for firmware and storage
  - Dual USB-C ports:
    - Port 1: Programming/debugging (UART)
    - Port 2: Power input
  - Built-in Wi-Fi for Matter connectivity
  - 5V power output for PIR sensor
- HC-SR501 PIR sensor for motion detection:
  - Maximum sensitivity setting for long-range detection
  - Optimized for ~45' detection range
  - Lens oriented for person-door coverage
  - Mounted high for maximum coverage area
  - Powered by ESP32's 5V output
- Status LEDs:
  - Power indicator (solid when active)
  - Network/Matter status (blinking during setup, solid when connected)
  - Motion detection (brief flash on trigger, disabled during normal operation)
  - Minimal brightness for normal operation
  - Full brightness during debug/setup
- QR code label for Matter setup

### 2. Power Management
- USB-C power input:
  - 5V operation
  - Connected to dedicated power USB-C port
  - Powers both ESP32 and PIR sensor
- Clean power distribution:
  - 5V direct to PIR sensor
  - 3.3V regulated for ESP32 core
  - LED power management
- Programming port remains accessible:
  - Separate USB-C port for updates
  - No interference with power delivery
  - Allows live debugging

### 3. Motion Detection
- PIR sensor configuration:
  - Maximum sensitivity for range
  - Wide detection angle (~120°) for coverage
  - Stable triggering at distance
  - Light interference mitigation:
    - Account for garage door opener activation (4x60W bulbs)
    - Handle light state changes from ceiling lights (2x100W)
    - Calibration for door operation light changes
- Motion timeout handling:
  - Default timeout: 15 minutes
  - Configurable range: 5-30 minutes via Home app
  - "Occupied" state triggers immediately on motion
  - "Unoccupied" state only after timeout period
  - Timer resets on any new motion detection
- Occupancy state management:
  - Initial state: unoccupied
  - Immediate transition to occupied on motion
  - Delayed transition to unoccupied after timeout
  - State changes trigger Matter updates
- False positive mitigation:
  - Light change filtering
  - EMI shielding from nearby opener
  - Motion pattern analysis
  - Debouncing
- Coordination with garage door events:
  - Expect light changes during door operation
  - Filter out door motion

### 4. Matter Integration
- Matter protocol implementation
- Apple Home compatibility:
  - Binary occupancy sensor type
  - Control of ceiling mounted lights (2x100W)
- QR code generation and placement
- Secure commissioning support
- Light automation:
  - Turn on ceiling lights on occupancy
  - Integration with Home automations
  - Consider light state during door operation

### 5. Environmental Considerations
- Appropriate enclosure for garage environment:
  - Heat dissipation
  - Dust protection
  - Clear PIR sensor window
  - Stable mounting solution
- Dust protection for electronics

## Success Criteria
1. Reliable motion detection at 45' range
2. Successful Matter integration with Apple Home
3. Stable operation on USB-C power
4. Quick setup via QR code
5. Accurate occupancy reporting
6. Less than 1 false reading per day

## Development Priorities

### MVP Phase
1. Basic hardware assembly and testing:
   - PIR sensitivity calibration
   - Power supply verification
   - USB-C power and programming setup
2. Power supply implementation
3. Motion detection configuration:
   - 5-minute timeout implementation
   - Light interference handling
4. Matter protocol integration
5. Initial Apple Home testing:
   - Occupancy detection
   - Ceiling light control

### Future Phases
1. Over-the-air firmware updates
2. Enhanced status indicators
3. Additional sensor capabilities
4. Configuration options
5. Advanced automation features