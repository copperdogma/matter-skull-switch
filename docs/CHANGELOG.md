# Changelog

20250117: Hardware Specification Updates
- Verified HC-SR501 PIR sensor capabilities
- Removed temperature sensing feature (not supported by hardware)
- Updated documentation to reflect hardware capabilities

20250116: Initial Project Setup
- Created project documentation structure
- Defined core requirements and specifications
- Established technical design approach
- Selected key components:
  * ESP32-S3 WROOM as main controller
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