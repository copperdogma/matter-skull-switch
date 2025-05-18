# Scratchpad

### Current Phase
- Implementing Matter features for Occupancy Sensor.

### Next Steps
- [x] **Configure Occupancy Sensor Delay Attribute:**
    - **Goal:** Allow the `PIROccupiedToUnoccupiedDelay` attribute of the OccupancySensing cluster to be configured by a Matter controller (e.g., Apple Home) and have this value persist and be used by the sensor.
    - **Tasks:**
        1.  **Identify Target Attribute:** (Completed)
            *   Primary: `PIROccupiedToUnoccupiedDelay` (ID `0x0010`, `uint16`, seconds).
            *   Project Requirement: Default 15 minutes (900s), configurable 5-30 minutes (300s-1800s).
        2.  **Ensure Attribute in ESP-Matter Data Model (`app_main.cpp`):** (Completed)
            *   Added logic to `app_main.cpp` to create/verify `PIROccupiedToUnoccupiedDelay` attribute on endpoint 1, OccupancySensing cluster.
            *   Set to `WRITABLE | NONVOLATILE` with a default of 900s if uninitialized.
        3.  **Handle Attribute Updates (in `app_attribute_update_cb` in `app_driver.cpp`):** (Completed - Indirectly)
            *   No direct action needed in callback for this attribute. Persistence is handled by `NONVOLATILE` flag. PIR driver reads value dynamically.
        4.  **Update PIR Sensor Logic (e.g., in `app_main.cpp` or `drivers/pir_sensor.c`):** (Completed)
            *   Added `get_pir_unoccupied_delay_seconds(uint16_t endpoint_id)` C-callable function in `app_main.cpp`.
            *   Modified `drivers/pir_sensor.c` to call this function to get the delay for its timer.
        5.  **Testing:** (Pending - User Task)
            *   Commission with a Matter controller (e.g., Apple Home).
            *   Verify the delay setting is exposed and configurable.
            *   Change the value via the controller.
            *   Confirm through device logs or behavior that the new delay is used.
            *   Reboot the ESP32 and confirm the setting persists and is used.

### Completed Tasks (Recent First)
- Implemented configurable `PIROccupiedToUnoccupiedDelay` attribute for Occupancy Sensor.
  - Modified `app_main.cpp` to create/manage the Matter attribute.
  - Added a C-callable helper in `app_main.cpp` to get the delay value.
  - Modified `drivers/pir_sensor.c` to use the configurable delay from Matter.
- Researched how to configure the OccupancySensing cluster's delay attributes.

### Issues or Blockers
- None currently for this specific task.

### Decisions Made
- The `PIROccupiedToUnoccupiedDelay` attribute will be made writable and non-volatile.
- Default delay will be set to 15 minutes (900 seconds). **Decision Rationale:** This value balances user convenience (especially for garage-like spaces where someone might be still for periods) and energy savings, and aligns with common industry practices. It remains configurable via Matter for future adjustments if needed, even if not directly exposed in all controller UIs like Apple Home.
- Apple Home does not currently offer a direct UI control for `PIROccupiedToUnoccupiedDelay`, but the attribute is supported by the Matter OccupancySensing cluster specification.

### Reference Information
- **ESP32 Device Node ID (from logs):** `0x000000002FF3D579` (Hex) / `133000000057` (Decimal)
### Log of Recent Actions & Status
- Researched if Apple Home supports UI for `PIROccupiedToUnoccupiedDelay`: Conclusion - Not directly in the UI, but the attribute is part of the Matter spec.
- Researched and decided on a default occupancy timeout: 15 minutes (900 seconds).
- Updated `README.md` with the default timeout and reasoning.
- Updated `firmware/main/app_main.cpp` to ensure the default for `PIROccupiedToUnoccupiedDelay` is 900s (no change needed as it was already this value).

### Next Steps
- NOT DOING THIS: **Address `chip-tool` connectivity/discovery issues:**
    - **Goal:** Successfully use `chip-tool` to read and write the `PIROccupiedToUnoccupiedDelay` attribute.
    - **Background:** The last attempt to read the attribute with `chip-tool` resulted in extensive logs indicating potential network discovery or commissioning issues, even though the Node ID and Endpoint ID were correct. The tool seemed to be trying to commission a new fabric rather than connect to an existing commissioned device.
    - **Troubleshooting Steps:**
        1.  **Verify Commissioning:** Ensure the ESP32 is still properly commissioned to a Matter fabric that `chip-tool` has credentials for. If `chip-tool` is on a different fabric or doesn't have the right operational credentials, it won't be able to communicate. The verbose output suggested it was trying to create a *new* fabric for itself (`Added new fabric at index: 0x1`, `Assigned compressed fabric ID...`).
        2.  **Check Network:** Confirm both the machine running `chip-tool` and the ESP32 are on the same Wi-Fi network and can reach each other (e.g., try pinging the ESP32's IP address if known).
        3.  **`chip-tool` Fabric Scope:** When `chip-tool` is used, it operates within the context of a specific fabric. If your ESP32 was commissioned by Apple Home, `chip-tool` needs to be aware of that fabric's credentials. This might involve:
            *   Using `chip-tool pairing already-commissioned` if you have the necessary details.
            *   Ensuring the `chip_tool_kvs` (Key-Value Store, typically in `/tmp/chip_tool_kvs` or similar) used by `chip-tool` contains the correct fabric information. The logs indicate it's using `/var/folders/8f/3nlcf3sj1s5bbk1g_3dt3djm0000gn/T//chip_tool_kvs`.
        4.  **Review `chip-tool` logs closely:** The ESP32 logs (`I (69117) chip[CTL]: Handling command for fabric index 1 node ID 0x0000000000000000`) show the device receiving *something*, but the `chip-tool` side fails before meaningful interaction.
        5.  **Simplify `chip-tool` Command:** Try a very basic read, like the vendor ID from the Basic Information cluster, to see if any communication is possible:
            `chip-tool generalinformation read vendor-id <NODE_ID_DECIMAL> 0`
            `chip-tool basicinformation read vendor-id <NODE_ID_DECIMAL> 0` (Try both `generalinformation` and `basicinformation` as cluster names can vary slightly in `chip-tool` usage).
- **Test actual delay behavior:** Once `chip-tool` can modify the attribute, set a short delay (e.g., 10 seconds) and a long delay (e.g., 1 minute) and observe if the PIR sensor's `g_occupancy_state` and subsequent Matter reports respect this changed delay.
- Consider Kconfig warnings for `SEC_CERT_DAC_PROVIDER` and `ENV_...` symbols if they become problematic. For now, they are non-fatal.
---

**Completed Tasks (from previous phases or this session):**
- [x] Initial research and planning for `PIROccupiedToUnoccupiedDelay`.
- [x] Added `PIROccupiedToUnoccupiedDelay` attribute to `app_main.cpp` (creatable, writable, non-volatile, default 900s).
- [x] Created C-callable wrapper `get_pir_unoccupied_delay_seconds` in `app_main.cpp`.
- [x] Modified `pir_sensor.c` to use `get_pir_unoccupied_delay_seconds`.
- [x] Successfully built the firmware with these changes.
- [x] Identified ESP32 Node ID from device logs: `0x000000002FF3D579` (Hex) / `133000000057` (Decimal).
- [x] Investigated Apple Home UI support for `PIROccupiedToUnoccupiedDelay` (Conclusion: No direct UI, but attribute is supported by Matter).
- [x] Researched and documented a suitable default for `PIROccupiedToUnoccupiedDelay` (15 minutes / 900 seconds) in `README.md`.

