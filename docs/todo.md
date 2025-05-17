# Project Todo List

20250116: Created by Cam Marsollier and Claude Sonnet 3.5
20250116: Updated by Cam Marsollier with Claude Sonnet 3.5 to add ESP32 Matter and PIR specifics
20250117: Updated by Cam Marsollier with Claude Sonnet 3.5 to reflect Matter cluster research and power simplification
20250117: Updated by Cam Marsollier with Claude Sonnet 3.5 to optimize task ordering and dependencies

## Temp/Next
- [ ] I think 8192 stack size is too large; I think it says that in console output, too
- [ ] I think all of these were added as debugging attempts previously. Do we need any of them?
      Under Component config -> ESP Matter:
      Device Info Provider options: Device Info - Custom
      Keep "Use ESP-Matter data model" CHECKED
      Enable Matter Server: Checked
      Initialize Thread stack...: Unchecked (since we're using Wi-Fi)
      In the Commissioning parameters section (if present), leave the default passcode/discriminator as-is
- [ ] After it works, let's try setting it up with the bare minimum of menuconfig changes to dial in what's essential.
- [x] This was an ongoing conversation: (Covered in SETUP.MD & README.md)
  - I see the issue - there's a mismatch between the target chip configuration. The build is occurring for esp32 instead of esp32s3, which is why the flash failed with the error: "This chip is ESP32-S3 not ESP32. Wrong --chip argument?"
  - `source ~/esp/esp-idf/export.sh && source ~/esp/esp-matter/export.sh && idf.py set-target esp32s3 reconfigure`
  - Note: Instructions for sourcing env and setting target are now in SETUP.MD and README.md.
- [ ] QR Code: Let's get our own device-specific QR code set up. I don't want to use the generic type-in code of 20202021. And I want us to be able to add the device with a QR code.

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
  - [ ] Upload basic firmware to ESP32-S3
    - idf.py -p /dev/tty.usbmodem101 flash monitor
  - [ ] Hello World test program (Can start locally with PlatformIO, then build in Docker)
  - [ ] **Implement PIR sensor driver and logic:**
    - [ ] Write/Adapt PIR sensor driver (`drivers/pir.c` or similar).
    - [ ] Implement interrupt handling for PIR sensor.
    - [ ] Update OccupancySensing cluster on Endpoint 1 based on PIR state.
    - [ ] Test PIR detection and Matter attribute updates.
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

## Console Log Analysis - High Priority Issues (2025-05-17)
- [ ] **Monitor (Previously Critical):** `E (587) esp_matter_cluster: Config is NULL or mandatory features are missing.`
    - Context: Occurs immediately after `factory_reset_button_register()` and during `esp_matter::node::create(&node_config, ...)`. Potentially indicates an issue with root node (Endpoint 0) or its mandatory cluster (Basic Info, Identify, Descriptor) initialization.
    - Current Status: The error persists. However, the device appears to commission and function as an occupancy sensor in the Home app. This error is now considered lower priority but should be monitored. If other issues arise that could be related to Endpoint 0 instability, this error might need to be revisited.
    - Github file where this error is thrown in the esp-matter repo: https://github.com/espressif/esp-matter/blob/057ac6a3e06f29a8a6f0e942228d01064e6942d3/components/esp_matter/esp_matter_cluster.cpp#L702
    - Attempts Made & Failed:
        - Zero-initializing `node_config{}`. This relies heavily on Kconfig for mandatory cluster defaults.
        - Explicitly setting `node_config.root_node.basic_information` members (e.g., `vendor_name`, `product_name`, `node_label`). Caused build errors if members didn't exist directly in `esp_matter::cluster::basic_information::config_t`.
        - Explicitly setting `node_config.root_node.identify` members (e.g., `identify_type`, `cluster_revision`). Caused build errors as `identify` is not a direct member of `esp_matter::endpoint::root_node::config_t`.
        - Explicitly setting `node_config.root_node.basic_information.cluster_revision` and `node_config.root_node.identify.cluster_revision / identify_type`. Caused build errors due to incorrect struct member access.
        - Reverted to simplest `node::config_t node_config{};` which builds but error persists.
        - Investigated and fixed linker errors related to `app_reset_button_register` by copying `app_reset.h` and `app_reset.cpp` into `firmware/main` and updating `main/CMakeLists.txt`. This fixed the build but did not resolve the "Config is NULL" error, indicating they are separate issues. The `app_reset` functionality itself is not the cause of the "Config is NULL" error.
    - Key Learnings & Suspicions:
        - The error "Config is NULL or mandatory features are missing" strongly points to an incomplete or misconfigured mandatory cluster on the root endpoint (Endpoint 0). These include Descriptor, Basic Information, Identify, General Commissioning, and Network Commissioning.
        - Since `node_config` in `app_main.cpp` is zero-initialized, the configuration for these root node clusters is primarily derived from Kconfig settings.
        - If `Component config -> ESP Matter -> Device Info Provider options` is set to `Device Info - Custom` (as it currently is), the settings under `Component config -> ESP Matter -> Device Basic Information` (like VID, PID, Vendor Name, Product Name, serial number, hardware/software versions) become critical and **must be fully populated**. Blank or incomplete entries here are a very likely cause of the error.
    - Next Steps to Attempt:
        - **Deep Dive into Kconfig for Basic Information:**
            - Verify **ALL** settings under `Component config -> ESP Matter -> Device Basic Information`. Ensure Vendor Name, Product Name, Hardware Version String, Software Version String, and Serial Number are explicitly set and not blank.
            - Confirm Vendor ID (VID) and Product ID (PID) in this section are correct.
- [ ] **Investigate:** `chip[ZCL]: WRITE ERR: ep 0 clus 0x0000_0030 attr 0x0000_0000 not supported`
    - Context: Logged after commissioning. Indicates an attempt to write to the OnOff attribute (0x0000) of the OnOff cluster (0x0030) on Endpoint 0 (Root Node). Endpoint 0 should not have an OnOff cluster.
    - Possible Causes:
        - Incorrect Descriptor cluster definition for Endpoint 0 (unlikely if using SDK defaults for Root Node type 22).
        - Controller (e.g., Home app) caching old state or probing standard clusters.
    - Action:
        - Monitor if this error persists, especially after factory resets and re-commissioning.
        - If problematic, ensure Endpoint 0 Descriptor does not list OnOff.
        - This is secondary to PIR sensor functionality but important for full compliance.
- [ ] **Error:** `E (77757) chip[SC]: The device does not support GetClock_RealTimeMS() API: 6c. Falling back to Last Known Good UTC Time` (Repeated at `E (86917)`)
    - Context: Occurs during secure session (CASE) establishment. Indicates the underlying platform (ESP-IDF) isn't providing an expected millisecond-precision real-time clock API. The Matter stack needs accurate time for certificate validation and other functions. Error `6c` suggests `CHIP_ERROR_UNSUPPORTED_CHIP_FEATURE`.
    - Investigation: Confirmed Freenove ESP32-S3 has a 32kHz crystal. Changed RTC clock source to "External 32 kHz crystal" in `menuconfig`, but this did not resolve the warning. SNTP is enabled. **Reverted RTC clock source to the default "Internal 136 kHz RC oscillator" as the device functions correctly with it and the change did not impact the warning.**
    - Status (YYYY-MM-DD): **Commissioning is successful and device operates as expected despite this error still appearing in logs.** The error is now considered a low-priority warning. See GitHub issue: [https://github.com/espressif/esp-matter/issues/1095](https://github.com/espressif/esp-matter/issues/1095)
- [ ] **Missing Diagnostics Attributes:** `E (81077) chip[DMG]: Read request on unknown cluster - no data version available` (and similar for cluster `0x0000_0046` - Software Diagnostics).
    - Context: Home app attempts to read attributes from Software Diagnostics cluster (`0x0046`) on Endpoint 0 (e.g., `CurrentHeapUsed`, `ThreadMetrics`), but device reports errors like "unknown cluster" or "no data version available". This means the cluster isn't present or fully initialized on Endpoint 0.
    - Action: 
        - Decide if these diagnostics are needed. If yes (recommended for robust devices):
            - In `app_main.cpp` (or relevant config code), ensure the Software Diagnostics cluster is added to Endpoint 0 using `esp_matter::cluster::software_diagnostics::create()`.
            - Provide a valid `config_t` for this cluster, including `cluster_revision` and a `feature_map` (e.g., to enable `WaterMark` or `ThreadMetrics` features if desired).
            - Implement the requested attributes (e.g., `CurrentHeapUsed`, `ThreadMetrics`) using `esp_matter` helpers like `esp_matter::cluster::software_diagnostics::attribute::create_current_heap_used()` and ensure the firmware can supply these values.
        - If No (simpler, but may still cause controller warnings): Ensure Endpoint 0's Descriptor Cluster correctly lists only the active clusters. A minimal Software Diagnostics cluster (ID, revision, empty feature map) might also reduce errors.
        - This is lower priority than the critical `esp_matter_cluster` error but good for full Matter compliance and reducing controller-side warnings.
- [ ] **Performance Note:** `E (79617) chip[DL]: Long dispatch time: 722 ms, for event type 3`
    - Context: Occurred during PASE (passcode-based commissioning, event type 3). A dispatch time of 722ms for an event is quite long and suggests potential CPU load or blocking behavior during cryptographic operations.
    - Action: 
        - **Primary:** Prioritize fixing the critical `E (587) esp_matter_cluster: Config is NULL...` error. System instability from this could contribute to performance issues.
        - **Monitor:** After critical errors are resolved, re-test commissioning and observe if this long dispatch time warning persists or is reduced.
        - **If Persists (Lower Priority):**
            - Check ESP-IDF `menuconfig` to ensure optimal CPU frequency is set (`Component config` -> `ESP32S3-specific` -> `CPU frequency`).
            - Review Matter task priorities and application task priorities to prevent starving the Matter event loop.
            - Consider reducing log verbosity during commissioning if it's currently high.
        - This is likely a symptom or secondary concern. Focus on core functionality and stability first. 

- [ ] **Build Output Warnings:** Kconfig definitions show redundancy and potential conflicts.
    - Context: Build output includes warnings like `default on the choice symbol SEC_CERT_DAC_PROVIDER ... will have no effect` and `the choice symbol SEC_CERT_DAC_PROVIDER ... is defined with a prompt outside the choice`. Additionally, multiple symbols (`SEC_CERT_DAC_PROVIDER`, `ENV_MAX_LEDS`, `ENV_GPIO_RANGE_MIN`, `ENV_GPIO_RANGE_MAX`, `ENV_GPIO_IN_RANGE_MAX`, `ENV_GPIO_OUT_RANGE_MAX`) are reported as defined in multiple locations.
    - Action:
        - Investigate the Kconfig files to resolve these warnings and informational messages.
        - Ensure `SEC_CERT_DAC_PROVIDER` is correctly and singularly defined, as it's critical for Matter device attestation. This could be linked to the `E (587) esp_matter_cluster: Config is NULL or mandatory features are missing` runtime error.
        - Consolidate or clarify the definitions for the `ENV_...` symbols within `espressif__esp_bsp_generic/Kconfig` or other relevant Kconfig files to remove redundancy. 