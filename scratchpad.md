# Scratchpad

## ✅ COMPLETED PROJECT STATUS

**Current Phase:** Project Documentation Migration - Generic Switch to Skull Switch
- ✅ Project base forked from Matter Generic Switch  
- ✅ Repository transformed to **matter-skull-switch** 
- ✅ ESP32-C3 SuperMini hardware selected for animatronic skull control
- ✅ Documentation completely rewritten for new purpose (README, SETUP, spec, circuit diagram)
- ✅ Created skull-integration.md with detailed animatronic integration guide

### Current Project Status
The project has been successfully migrated from a generic Matter switch to a specialized animatronic skull controller. All major documentation has been updated to reflect the new purpose and GPIO output functionality.

### Project Purpose
This ESP32-C3 SuperMini-based Matter switch serves as a wireless trigger for animatronic skulls:
1. **Matter Integration:** Appears as standard on/off switch in Apple Home, Google Home, Alexa
2. **GPIO Signal Output:** Sends 3.3V "GO!" signal to connected animatronic controllers  
3. **Trigger Behavior:** When switched "on" via Matter, sends configurable pulse to trigger audio playback
4. **Installation:** Sits inside skull enclosure alongside main animatronic electronics

---

### Hardware Configuration

**Final Hardware:**
*   **Microcontroller:** ESP32-C3 SuperMini
*   **Signal Output:** GPIO 4 (3.3V output to animatronic controller)

*   **Power:** USB-C 5V input (future: LiPo battery with TP4056 charging)

**Pin Configuration:**

*   **GPIO Signal Output (`SIGNAL_GPIO_NUM`):** GPIO 4

*   **Factory Reset:** GPIO 9 (BOOT button, built-in)

**Connections to Animatronic Controller:**

1.  **Signal Line**: ESP32-C3 GPIO 4 -> Animatronic Controller GPIO input
2.  **Ground**: ESP32-C3 GND -> Animatronic Controller GND  
3.  **Power (optional)**: ESP32-C3 3.3V -> Animatronic Controller VCC (if needed)

**Signal Behavior:**
*   **Matter "ON" Command:** GPIO 4 goes HIGH (3.3V) for configured duration (default 500ms)
*   **Matter "OFF" Command:** GPIO 4 goes LOW (0V)
*   **Pulse Mode:** Sends pulse when activated, returns to LOW
*   **Toggle Mode:** GPIO state follows Matter switch state

**Powering the System:**
*   Current: Via USB-C connection to ESP32-C3 SuperMini
*   Future: LiPo battery with TP4056 USB-C charging module + voltage monitoring

### Current Hardware Plan

* **Microcontroller:** ESP32-C3 SuperMini
* **Output Device:** GPIO signal to animatronic controller

* **Power:** USB-C 5 V (future: LiPo battery)

| Signal | ESP32-C3 Pin | Notes |
|--------|--------------|-------|
| Signal Output | GPIO 4 | 3.3V pulse to animatronic controller |

| Animatronic Signal | GPIO 4 | Connected to animatronic GPIO input |
| Shared Ground | GND | Common ground with animatronic |

---

### Matter Device Configuration

**Device Type:** On/Off Switch (not Generic Switch)
**Matter Clusters:**
- Basic Information (mandatory)
- Identify (mandatory)  
- On/Off (core functionality)
- Power Source (future: battery level reporting)

**Device Identification:**
- Vendor ID: 0xFFF1 (test vendor)
- Product ID: 0x8001 (skull switch)
- Device Name: "Skull Switch"

### Implementation Notes

1. **GPIO Output Control:** Main functionality is Matter on/off commands controlling GPIO 4 output
2. **Signal Timing:** Configurable pulse duration (50ms to 5000ms), default 500ms
3. **Network Latency:** Target <500ms from app tap to GPIO activation
4. **Multiple Devices:** Each skull needs unique certificates and commissioning

### Current Documentation Status

✅ **Updated Files:**
- README.md - Complete rewrite for skull switch functionality
- SETUP.md - Updated for skull switch development environment 
- project-overview-and-build.mdc - Updated project scope and build process
- docs/spec.md - Complete technical specification rewrite
- docs/circuit_diagram.md - New GPIO output wiring diagram
- docs/skull-integration.md - NEW: Detailed animatronic integration guide

🔄 **Files Needing Updates:**
- docs/wiring.md - Needs update for GPIO output instead of button input
- docs/requirements.md - Needs rewrite for skull switch requirements  
- docs/design.md - Needs complete rewrite for new architecture
- docs/todo.md - Update with current project tasks

❌ **Files to Consider Removing:**
- Outdated documentation that doesn't apply to skull switches
- PIR sensor references
- Occupancy sensor specific content

### Known Limitations (Current State)

1. Existing firmware still implements generic switch; needs updating for on/off switch
2. GPIO output functionality needs implementation in app_main.cpp
3. Device type configuration needs updating in firmware
4. Matter clusters need adjustment (remove switch events, add on/off)

### Future Enhancement Possibilities

* **Battery Power:** LiPo with USB-C charging and Matter battery level reporting
* **Scene Cluster:** Multiple trigger patterns for different effects  
* **Multiple GPIO Outputs:** Control multiple animatronic devices from one switch
* **Pulse Patterns:** Different signal patterns for various effect types
* **OTA Updates:** Over-the-air firmware updates
* **Custom PCB:** Integrated charging and better form factor for skull installation

### Multiple Skull Setup Strategy

For controlling multiple animatronic figures:
1. Each ESP32-C3 gets unique device certificates (different serial numbers)
2. Flash each device with its own factory NVS partition  
3. Commission each device separately in Matter controller app
4. Use Matter scenes to coordinate multiple skulls for synchronized effects
5. Each skull switch controls its own animatronic controller independently

---

