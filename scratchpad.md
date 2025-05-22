# Scratchpad

### Current Phase
- Implementing Matter features for Occupancy Sensor.

### Next Steps
- **Test actual delay behavior:** Once `chip-tool` can modify the attribute, set a short delay (e.g., 10 seconds) and a long delay (e.g., 1 minute) and observe if the PIR sensor's `g_occupancy_state` and subsequent Matter reports respect this changed delay.
- Consider Kconfig warnings for `SEC_CERT_DAC_PROVIDER` and `ENV_...` symbols if they become problematic. For now, they are non-fatal.

---

