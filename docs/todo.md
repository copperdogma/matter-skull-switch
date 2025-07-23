# Project Todo List – Generic Switch Migration

- [ ] Update the QR code in various files, plus replace "pr-code-1234.png" and associated psd.
- [ ] Pull down resistor for GPIO 4 signal? On esp32-wrover side? This was suggested in @project-overview-and-build.mdc Why do we need this?
- [x] Get rid of references to the optional LED. We're not doing that. 
- [ ] I was just going to hard-wire the 5v/GND/GPIO4 to the esp32-c3 to avoid the need for a perfboard. It's such a simple circuit I thought that would be fine. The other end would have a Dupont connector that would go where the ultrasonic sensor used to go on the main perfboard. I'll just reprogram the main board to expect the signal on one of the old ultrasonic sensor pins. There would be one unused pin but no biggie. Thoughts?
- [ ] What is an optocoupler? Why would I use that? What are the benefits/drawbacks?
- [ ] Might it be possible to set up a single esp32 to control both the skull AND be a matter controller?