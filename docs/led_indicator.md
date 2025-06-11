# LED Status Indicator

The Matter Occupancy Sensor project includes an LED status indicator that provides visual feedback about the device's operational status.

## Overview

The LED indicator uses PWM (Pulse Width Modulation) via the ESP32-C3's LEDC peripheral to control LED brightness and blinking patterns. The LED provides three primary states:

1. **Dim Static Light** - Indicates the device is powered on and operating normally, but no motion is detected (unoccupied state).
2. **Quick Flash (3x)** - Indicates motion has just been detected. The LED flashes brightly 3 times in rapid succession.
3. **Bright Static Light** - Indicates the occupied state is active. The LED stays at full brightness during the entire occupancy period.

## Behavior Sequence

The LED indicator follows this behavior pattern:

1. **Start/Idle State**: LED is dim (~10% brightness)
2. **Motion Detected**: LED flashes brightly 3 times in quick succession (150ms intervals)
3. **Occupied State**: After flashing, LED stays bright (100% brightness) for the entire occupancy period
4. **Return to Idle**: When occupancy times out (after 10 seconds of no motion in testing, configurable), LED returns to dim

## Hardware Setup

The LED is connected to GPIO pin 6 by default (configurable via `menuconfig`). An appropriate current-limiting resistor should be used in series with the LED.

Suggested hardware connection:
- Connect the LED anode (longer lead) to the GPIO pin
- Connect the LED cathode (shorter lead) through a 100-220Ω resistor to ground

## API Reference

The LED indicator driver provides the following functions:

- `pir_led_indicator_init(gpio_num)` - Initialize the LED on the specified GPIO
- `pir_led_indicator_set_dim()` - Set LED to dim state (unoccupied)
- `pir_led_indicator_set_blink()` - Flash LED 3 times and then switch to bright
- `pir_led_indicator_set_bright()` - Set LED to full brightness (occupied)
- `pir_led_indicator_deinit()` - Deinitialize the LED and free resources

## Configuration

The GPIO pin used for the LED indicator can be configured using menuconfig:

```
idf.py menuconfig
```

Navigate to: "Occupancy Sensor Configuration" → "LED Indicator GPIO Pin Number"

Default settings:
- GPIO pin: 6
- PWM frequency: 5000 Hz
- Dim mode duty cycle: ~10% brightness
- Bright mode duty cycle: 100% brightness
- Blink period: 500 ms (2 Hz)

## Integration with Occupancy Detection

The LED indicator is automatically controlled based on the occupancy state:
- When motion is detected, the PIR sensor callback triggers the LED to enter bright blinking mode
- When motion is no longer detected, the LED returns to dim mode 