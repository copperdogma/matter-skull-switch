# LED Status Indicator

The Matter Generic Switch project includes an LED status indicator that provides visual feedback about the device's operational status.

## Overview

The LED indicator uses PWM (Pulse Width Modulation) via the ESP32-C3's LEDC peripheral to control LED brightness and blinking patterns. The LED provides three primary states:

1. **Dim Static Light** – Device powered on, no recent switch activity.
2. **Quick Flash (3×)** – Button was just pressed. The LED flashes brightly three times.
3. **Bright Static Light** – Button is being held down (optional) or within the debounce window.

## Behavior Sequence

The LED indicator follows this behavior pattern:

1. **Start/Idle State**: LED is dim (~10% brightness)
2. **Button Pressed**: LED flashes brightly three times (150 ms intervals)
3. **Active State**: Optionally, LED stays bright (100 % brightness) while the button is held.
4. **Return to Idle**: After the button is released (or after a configurable timeout), LED returns to dim.

## Hardware Setup

The LED is connected to GPIO pin 6 by default (configurable via `menuconfig`). An appropriate current-limiting resistor should be used in series with the LED.

Suggested hardware connection:
- Connect the LED anode (longer lead) to the GPIO pin
- Connect the LED cathode (shorter lead) through a 100-220Ω resistor to ground

## API Reference

The LED indicator driver provides the following functions:

- `gs_led_indicator_init(gpio_num)` - Initialize the LED on the specified GPIO
- `gs_led_indicator_set_dim()` - Set LED to dim (idle) state
- `gs_led_indicator_set_blink()` - Flash LED three times quickly
- `gs_led_indicator_set_bright()` - Set LED to full brightness (active)
- `gs_led_indicator_deinit()` - De-initialize the LED and free resources

## Configuration

The GPIO pin used for the LED indicator can be configured using menuconfig:

```
idf.py menuconfig
```

Navigate to: "Generic Switch Configuration" → "LED Indicator GPIO Pin Number"

Default settings:
- GPIO pin: 6
- PWM frequency: 5000 Hz
- Dim mode duty cycle: ~10% brightness
- Bright mode duty cycle: 100% brightness
- Blink period: 500 ms (2 Hz)

## Integration with Switch Events

The LED indicator is automatically controlled based on switch events:
* When the button is pressed, the GPIO interrupt triggers the LED to flash then optionally stay bright.
* When the button is released (or after timeout), the LED returns to dim mode. 