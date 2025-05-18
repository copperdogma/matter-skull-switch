# Project Todo List

20250116: Created

## Next Steps


## High Priority

### Environment & Setup
- [x] Resolve outstanding questions from spec:
  - [x] Determine appropriate unoccupied delay time (15 minutes default, configurable 5-30 minutes)
  - [x] Decide if delay should be HomeKit configurable (Yes, via Matter attributes)
- [x] Development Environment Setup:
  - [x] Install ESP-IDF v5.1 or newer (via PlatformIO for local basic tests)
  - [x] Add `esp-matter` SDK as a project component
  - [x] Configure Matter development certificates (Generate test DAC)
  - [x] Configure version control (Git already initialized)
  - [x] Configure `sdkconfig` for Matter (Enable Matter, set device type)
  - [x] Set up proper .gitignore for certificates and sensitive data
  - [x] QR Code: Set up device-specific QR code instead of generic 20202021 code
- [x] [NOPE. This is the recommended size.] I think 8192 stack size is too large; I think it says that in console output, too


### Hardware Validation
- [x] Verify and document HC-SR501 PIR sensor specifications:
  - [x] Locate official manufacturer datasheet
  - [x] Verify operating parameters and limits
  - [x] Update design documentation with verified specs
- [ ] Complete ESP32-S3 WROOM documentation:
  - [x] Locate official pinout diagram (GitHub: Freenove/Freenove_ESP32_S3_WROOM_Board)
  - [x] Download technical documentation
  - [x] Verify GPIO capabilities and pin functions
  - [x] Document power pin locations
- [ ] Hardware Testing:
  - [x] Power up ESP32 via USB-C
  - [x] Test 5V output capability
  - [x] Basic PIR sensor functionality check (Verified 0V/3.3V output, 2.5s block time)
  - [ ] GPIO input test with PIR

### Firmware Implementation
- [x] **Implement PIR sensor driver and logic:**
  - [x] Write/Adapt PIR sensor driver (`drivers/pir_sensor.c` & `drivers/include/pir_sensor.h`)
  - [x] Implement interrupt handling for PIR sensor
  - [x] Update OccupancySensing cluster on Endpoint 1 based on PIR state
  - [x] Test PIR detection and Matter attribute updates
- [x] Basic Firmware Implementation:
  - [x] Upload basic firmware to ESP32-S3
  - [x] LED status indicators. Pick a GPIO pin, use PWM: dim for "on", bright blinking for "motion detected". using 220Ω resistor.
  - [ ] SKIPPING THIS: [Apple Home doesn't suport it in the UI] Basic state management

### Matter Integration
- [ ] Matter Integration:
  - [x] QR code provisioning
  - [x] HomeKit pairing validation
  - [x] OccupancySensing cluster (0x0406)
  - [x] Delay attribute configuration
  - [ ] SKIPPING THIS: [Apple Home doesn't suport it in the UI] Settings persistence

## Research Tasks
- [x] Research ESP32-S3 Matter examples/implementations:
  - [x] Study esp-matter occupancy sensor example
  - [x] Review Matter cluster specifications for occupancy
  - [x] Analyze ESP32 Matter commissioning flow
- [ ] Investigate PIR sensor mounting:
  - [ ] Test sensitivity ranges
  - [ ] Determine optimal mounting height/angle

## Documentation
- [x] Create project README
- [x] Document development setup process
- [ ] Create hardware assembly guide:
  - [ ] Wiring diagram
  - [ ] Final Wiring Plan:
    - [ ] Use Dupont connectors directly to ESP32 pins for PIR sensor and LED
    - [ ] Make GND pigtail to ESP32 GND because we need two ground connections (PIR sensor and LED)
    - [ ] Add small dab of non-conductive hot glue at connector bases for stability

## Testing
- [ ] Functionality Testing:
  - [ ] PIR sensitivity calibration
  - [ ] Motion detection reliability
  - [ ] Delay timing accuracy
  - [ ] Settings persistence
- [ ] Integration Testing:
  - [ ] Matter commissioning
  - [ ] HomeKit interaction
  - [ ] Power cycle recovery
  - [ ] Network reconnection

## Deferred / Lower Priority Items

- [ ] **Troubleshoot `chip-tool` Connectivity/Discovery Issues:**
    - Deferred until core sensor functionality with Apple Home is stable
    - Will need to investigate fabric issues between `chip-tool` and ESP32
    - Possible steps: verify fabric credentials, check controller Key-Value Store
    
- [ ] **Test Configurable Occupancy Delay Behavior Extensively:**
    - Dependent on resolving `chip-tool` connectivity to modify the attribute
    - Test various delay values and confirm behavior persistence after reboots

- [ ] **Address Kconfig Build Warnings:**
    - Non-fatal warnings about `SEC_CERT_DAC_PROVIDER` and other symbols
    - Consider cleaning up for polish but not critical for functionality

- [ ] NOT FIXING: **Monitor Console Warnings:** 
    - `E (587) esp_matter_cluster: Config is NULL or mandatory features are missing.`
    - Despite this error, device functions correctly in Apple Home

- [ ] NOT FIXING: **RTC Clock Warning:**
    - `E (77757) chip[SC]: The device does not support GetClock_RealTimeMS() API: 6c.`
    - Known issue in ESP-Matter (GitHub issue #1095), doesn't affect functionality

## Future Enhancements
- [ ] OTA update capability
- [ ] Enhanced status indicators
- [ ] Advanced automation features
- [ ] Matter security hardening
- [ ] Proper enclosure with temperature rating (-20°C to +70°C)