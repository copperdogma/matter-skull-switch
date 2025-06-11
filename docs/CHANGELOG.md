# Changelog

20250509: Initial Matter Setup
- Set up computer environment:
  brew install cmake ninja dfu-util
  python --version (shows me running 3.11.5 which is >=3 which is ok)
  brew install ccache
- Followed SETUP.md instructions to the letter.
  [2025-05-10 01:18:38,873] [   INFO] - Generated QR code: MT:Y.K90MBW17GU0Y7RD00
  [2025-05-10 01:18:38,873] [   INFO] - Generated manual code: 3321-531-0437
  [2025-05-10 01:18:38,882] [   INFO] - Generated onboarding data and QR Code



20250117: Initial Hardware Validation
- Completed PIR sensor testing:
  - Verified 0V/3.3V output levels
  - Confirmed 2.5s block time behavior
  - Documented signal characteristics
- Power system validation:
  - Tested ESP32 USB-C power
  - Verified 5V output capability
  - Documented power budget analysis
- Documentation updates:
  - Located and archived ESP32-S3 documentation (migrated to ESP32-C3)
  - Added detailed PIR test results
  - Consolidated power management documentation
  - Organized reference materials

20250117: Hardware Specification Updates
- Verified HC-SR501 PIR sensor capabilities
- Updated documentation to reflect hardware capabilities

20250116: Initial Project Setup
- Created project documentation structure
- Defined core requirements and specifications
- Established technical design approach
- Selected key components:
  * ESP32-C3 SuperMini as main controller
  * HC-SR501 PIR sensor for motion detection
  * Matter protocol for HomeKit integration
  * USB-C power delivery

Build notes:
- Researched ESP32 Matter implementation options
- Evaluated PIR sensor configurations for 45' range
- Investigated power delivery requirements
- Key decisions:
  * ESP-IDF v5.1+ required for Matter support
  * 50ms debouncing for motion detection
  * 5-minute occupancy timeout
  * Dual USB-C ports for power/programming
- Referenced ESP32 Motion Sensor Tutorial and Matter documentation
- Created initial documentation structure:
  * spec.md: Requirements and success criteria
  * design.md: Technical implementation details
  * todo.md: Active tasks and planning
  * CHANGELOG.md: Project history 