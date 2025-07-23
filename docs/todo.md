# Project Todo List – Generic Switch Migration

- [ ] Update the QR code in various files, plus replace "pr-code-1234.png" and associated psd.
- [x] Pull down resistor for GPIO 4 signal? On esp32-wrover side? This was suggested in @project-overview-and-build.mdc Why do we need this?
  - Not necessary because the esp32-c3 SuperMini is powered by the esp32-wrover so the boards power up and down together so there's no real possibility of the gpio pin floating high and causing a false signal. Be doubly-sure by setting the esp32-wrover receiving pin in setup to: pinMode(triggerPin, INPUT_PULLDOWN); // guarantees LOW at boot
- [x] Get rid of references to the optional LED. We're not doing that. 
- [ ] 90 degree male header for pins GND/3.3V/GPIO4. This will make a dupont connector simple.
- [x] Might it be possible to set up a single esp32 to control both the skull AND be a matter controller?
  - NOT really, not due to chip limitations but bluetooth limitations. I'd be using bluetooth for A2DP, BLE, AND matter. That's just too much for one chip. I could get a separate bluetooth module but the esp32-c3 SuperMini is tiny and cheap and already has everything we need, so might as well make it discrete and simplify. That also makes the matter component a pluggable component for any other project.