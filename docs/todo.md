# Project Todo List

20250116: Created by Cam Marsollier and Claude Sonnet 3.5
20250116: Updated by Cam Marsollier with Claude Sonnet 3.5 to add ESP32 Matter and PIR specifics
20250117: Updated by Cam Marsollier with Claude Sonnet 3.5 to reflect Matter cluster research and power simplification
20250117: Updated by Cam Marsollier with Claude Sonnet 3.5 to optimize task ordering and dependencies

## High Priority
- [x] Resolve outstanding questions from spec:
  - [x] Determine appropriate unoccupied delay time (15 minutes default, configurable 5-30 minutes)
  - [x] Decide if delay should be HomeKit configurable (Yes, via Matter attributes)
- [x] Verify and document HC-SR501 PIR sensor specifications:
  - [x] Locate official manufacturer datasheet
  - [x] Verify operating parameters and limits
  - [x] Confirm no temperature sensing capability
  - [x] Update design documentation with verified specs
- [ ] Basic Hardware Validation:
  - [ ] Power up ESP32 via USB-C
  - [ ] Test 5V output capability
  - [ ] Basic PIR sensor functionality check
  - [ ] GPIO input test with PIR
- [ ] Development Environment Setup:
  - [ ] Install ESP-IDF v5.1 or newer
  - [ ] Configure Matter development container
  - [ ] Install Matter SDK dependencies
  - [ ] Configure Matter development certificates
  - [ ] Set up debugging tools
  - [ ] Configure version control
- [ ] Basic Firmware Implementation:
  - [ ] Hello World test program
  - [ ] PIR interrupt handling
  - [ ] LED status indicators
  - [ ] Basic state management
- [ ] Matter Integration:
  - [ ] Basic device setup
  - [ ] QR code provisioning
  - [ ] HomeKit pairing validation
  - [ ] OccupancySensing cluster (0x0406)
  - [ ] Delay attribute configuration
  - [ ] Settings persistence

## Research Tasks
- [x] Research ESP32-S3 Matter examples/implementations:
  - [x] Study esp-matter occupancy sensor example
  - [x] Review Matter cluster specifications for occupancy
  - [ ] Analyze ESP32 Matter commissioning flow
- [ ] Investigate PIR sensor mounting:
  - [ ] Test sensitivity ranges
  - [ ] Evaluate lens coverage patterns
  - [ ] Determine optimal mounting height/angle

## Documentation
- [ ] Create project README
- [ ] Document development setup process
- [ ] Create hardware assembly guide:
  - [ ] Wiring diagram
  - [ ] Mounting instructions
  - [ ] Configuration steps
- [ ] Document Matter integration:
  - [ ] QR code generation process
  - [ ] HomeKit setup instructions
  - [ ] Configuration options

## Testing
- [ ] Functionality Testing:
  - [ ] PIR sensitivity calibration
  - [ ] Motion detection reliability
  - [ ] Delay timing accuracy
  - [ ] Settings persistence
- [ ] Environmental Testing:
  - [ ] Temperature range validation
  - [ ] Light interference mitigation
  - [ ] EMI susceptibility checks
- [ ] Integration Testing:
  - [ ] Matter commissioning
  - [ ] HomeKit interaction
  - [ ] Power cycle recovery
  - [ ] Network reconnection
- [ ] Long-term Testing:
  - [ ] Stability monitoring
  - [ ] False positive tracking
  - [ ] Power consumption analysis

## Future Enhancements
- OTA update capability
- Enhanced status indicators
- Advanced automation features
- Matter security hardening
- Additional sensor capabilities
- Proper enclosure:
  - Temperature rated (-20°C to +70°C)
  - Moisture resistant
  - 3D printed design
  - PIR lens integration 