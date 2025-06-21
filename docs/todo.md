# Project Todo List – Generic Switch Migration

*Created 2025-06-21 (migration from occupancy sensor project)*

## Immediate Tasks

1. **Firmware Refactor**
   - [ ] Run `idf.py menuconfig` and enable **Generic Switch** device type; disable Occupancy Sensor.
   - [ ] Remove PIR sensor driver and related logic from `app_main.cpp` and drivers folder.
   - [ ] Implement GPIO interrupt handler for momentary button on GPIO 3.
   - [ ] Map button events to Switch cluster attributes/events.

2. **Documentation Rewrite (phase 2)**
   - [ ] Rewrite `docs/design.md` focusing on switch architecture.
   - [ ] Replace `docs/spec.md` with a concise Generic Switch spec (requirements & success criteria).

3. **Hardware Validation**
   - [ ] Verify debouncing strategy (<10 ms).
   - [ ] Confirm push-button wiring and pull-up/down configuration.
   - [ ] Validate LED PWM on GPIO 5.

## Deferred
* Remote-control automations testing on Google Home & Alexa
* Battery-powered prototype using 18650 Li-ion + buck converter

---

## Archived Tasks (Occupancy Sensor)

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
- [x] Complete ESP32-C3 SuperMini documentation:
- [x] Locate official pinout diagram and references
  - [x] Download technical documentation
  - [x] Verify GPIO capabilities and pin functions
  - [x] Document power pin locations
- [ ] Hardware Testing:
  - [x] Power up ESP32 via USB-C
  - [x] Test 5V output capability
  - [x] Basic PIR sensor functionality check (Verified 0V/3.3V output, 2.5s block time)
  - [x] GPIO input test with PIR

### Firmware Implementation
- [x] **Implement PIR sensor driver and logic:**
  - [x] Write/Adapt PIR sensor driver (`drivers/pir_sensor.c` & `drivers/include/pir_sensor.h`)
  - [x] Implement interrupt handling for PIR sensor
  - [x] Update OccupancySensing cluster on Endpoint 1 based on PIR state
  - [x] Test PIR detection and Matter attribute updates
- [x] Basic Firmware Implementation:
  - [x] Upload basic firmware to ESP32-C3
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
- [x] Research ESP32-C3 Matter examples/implementations:
  - [x] Study esp-matter occupancy sensor example
  - [x] Review Matter cluster specifications for occupancy
  - [x] Analyze ESP32 Matter commissioning flow
- [x] Investigate PIR sensor mounting:
  - [x] Test sensitivity ranges
  - [x] Determine optimal mounting height/angle

## Documentation
- [x] Create project README
- [x] Document development setup process
- [x] Create hardware assembly guide:
  - [x] Wiring diagram (See [docs/circuit_diagram.md](../circuit_diagram.md))
  - [x] Final Wiring Plan:
    - [x] Use Dupont connectors directly to ESP32 pins for PIR sensor and LED
    - [x] Make GND pigtail to ESP32 GND because we need two ground connections (PIR sensor and LED)
    - [x] Add small dab of non-conductive hot glue at connector bases for stability

## Testing
- [x] Functionality Testing:
  - [x] PIR sensitivity calibration
  - [x] Motion detection reliability
  - [x] Delay timing accuracy
- [x] Integration Testing:
  - [x] Matter commissioning
  - [x] HomeKit interaction
  - [x] Power cycle recovery
  - [x] Network reconnection

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

## ESP32-C3 Supermini Migration 

- [x] **Update ESP-IDF target:**
    - [x] Added ESP32-C3 specific configuration to `Kconfig.projbuild`
    - [x] Created `sdkconfig.defaults.esp32c3` file
    - [x] Run `idf.py set-target esp32c3` to change target from ESP32-S3 to ESP32-C3
    - [x] Run `idf.py reconfigure` after changing the target

- [x] **Update GPIO pin configurations:**
    - [x] Researched ESP32-C3 Supermini pinout and safe pins to use
    - [x] Updated GPIO assignments in `Kconfig.projbuild` to match C3 Supermini pins:
      - PIR sensor moved from GPIO 4 to GPIO 3
      - LED indicator moved from GPIO 6 to GPIO 10
      - I2C pins set to GPIO 8 (SDA) and GPIO 9 (SCL)
    - [x] Avoided strapping pins (GPIO 2, 8, 9) for critical functions
    - [x] Avoided JTAG pins (GPIO 4-7) for critical functions
    - [x] Test the new pin assignments on actual hardware

- [x] **Memory and performance optimization:**
    - [x] Added optimizations to `sdkconfig.defaults.esp32c3`:
      - Reduced boot time with logging level adjustments
      - Memory optimization for single-core operation
      - Disabled SPIRAM to match C3 hardware capabilities
      - Adjusted partition table for 4MB flash

- [x] **Platform-specific feature adjustments:**
    - [x] Ensured configuration is compatible with C3's RISC-V architecture
    - [x] Verify PWM/LEDC implementation for LED indicator works on C3
  
- [x] **Build and test changes:**
    - [x] Build with ESP32-C3 target
    - [x] Test basic functionality
    - [x] Verify Matter commissioning still works
    - [x] Verify occupancy detection and reporting

- [x] **Documentation updates:**
    - [x] Updated README.md with ESP32-C3 support information
    - [x] Added hardware connection details for ESP32-C3 Supermini
    - [x] Add physical wiring diagram for ESP32-C3 setup (See [docs/circuit_diagram.md](../circuit_diagram.md))

## Future Enhancements
- [ ] OTA update capability
- [ ] Enhanced status indicators
- [ ] Advanced automation features
- [ ] Matter security hardening
- [ ] Proper enclosure with temperature rating (-20°C to +70°C)

## Suggested Final Tasks
- [x] Final Reliability Testing:
    - [x] Power cycle test (confirm functionality and settings persistence).
    - [x] Network reconnection test (confirm recovery after Wi-Fi disruption).
- [x] Finalize `sdkconfig.defaults.esp32c3`:
    - [x] Copy current working `firmware/sdkconfig` to `sdkconfig.defaults.esp32c3` after all settings are finalized.