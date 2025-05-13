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
  - [x] Update design documentation with verified specs
- [ ] Obtain and document Freenove ESP32-S3 WROOM specifications:
  - [x] Locate official pinout diagram and documentation (GitHub: Freenove/Freenove_ESP32_S3_WROOM_Board)
  - [x] Download technical documentation/datasheet
  - [x] Verify GPIO capabilities and pin functions
  - [x] Document power pin locations
- [ ] Basic Hardware Validation:
  - [x] Power up ESP32 via USB-C
  - [x] Test 5V output capability
  - [x] Basic PIR sensor functionality check (Verified 0V/3.3V output, 2.5s block time)
  - [ ] GPIO input test with PIR
- [x] Development Environment Setup:
  - [x] Install ESP-IDF v5.1 or newer (via PlatformIO for local basic tests)
  - [x] Set up Matter Docker container (Pulled espressif/esp-matter image and created start script) - **Primary environment for Matter development**
  - [x] Add `esp-matter` SDK as a project component (Inside Docker)
    - Note: Added `espressif/esp_matter^1.4.0` as a dependency to a dummy `main` component using `idf.py add-dependency` inside the running Matter Docker container. Verified by a successful PlatformIO build from the IDE.
  - [x] Configure Matter development certificates (Generate test DAC) (Inside Docker)
    - Generated test certificates and factory partition using `chip-cert` for CD and PAA, then `esp-matter-mfg-tool` for DAC/PAI and factory partition. Files stored in `matter_certs/` directory.
  - [ ] MAKE SURE .gitignore is set up properly becuase we've created a LOT of stuff that we don't want to commit. QR codes? certificates? etc
  - [ ] MAYBE LATER: Set up debugging tools (Configure `platformio.ini` for ESP32-S3 JTAG, potentially use with Docker)
  - [ ] Configure `sdkconfig` for Matter (Enable Matter, set device type) (Inside Docker via `idf.py menuconfig`)
  - [ ] Review and finalize `platformio.ini` settings (For both local basic tests and Docker interaction)
  - [x] Configure version control (Git already initialized)

**--- Development Environment Transition Point ---**
*For tasks below this line, all development, compilation, and flashing should be performed INSIDE the Matter Docker container using `idf.py` commands.*
*Code editing can still be done in your local IDE (Cursor), as the project directory is mounted into the container.*

- [ ] Basic Firmware Implementation (Phase 1 - Initial tests in Local PlatformIO, then transition to Docker for Matter integration):
  - [ ] Hello World test program (Can start locally with PlatformIO, then build in Docker)
  - [ ] PIR interrupt handling (Basic GPIO read locally, full interrupt logic in Docker with ESP-IDF)
  - [ ] LED status indicators (Basic control locally, more complex status with Matter in Docker)
  - [ ] Basic state management (Initial logic locally, integrate with Matter states in Docker)
- [ ] Matter Integration (All tasks to be performed INSIDE Docker container):
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