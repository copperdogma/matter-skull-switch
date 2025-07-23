# ESP32-C3 Matter Skull Switch – Design Document

> **Last updated:** 2025-07-22

## Overview
This document details the technical design and implementation of the ESP32-C3 SuperMini Matter Skull Switch. The goal is to meet the requirements in `spec.md` by providing a reliable, Matter-compatible On/Off switch that triggers animatronic skulls and effects via a GPIO output signal.

---

## 1. System Architecture

### 1.1. High-Level Block Diagram
```
[Matter Controller]
   (Apple Home, Google Home, Alexa)
         |
         |  (Wi-Fi / Matter)
         v
[ESP32-C3 SuperMini]
   |  GPIO4 ("GO!" signal)
   v
[Animatronic Controller (ESP32, etc.)]
   |  Audio/Servo/Effect
   v
[Animatronic Skull]
```

### 1.2. Key Components
- **ESP32-C3 SuperMini**: Main microcontroller, runs ESP-IDF and ESP-Matter SDK
- **GPIO Output (GPIO 4)**: Sends 3.3V "GO!" pulse to animatronic controller

- **USB-C Power**: 5V input, regulated to 3.3V for logic
- **Animatronic Controller**: Receives "GO!" signal, plays audio/animates skull

---

## 2. Hardware Design

### 2.1. Microcontroller: ESP32-C3 SuperMini
- **Features**: Wi-Fi, BLE 5.0, 400KB SRAM, 4MB Flash
- **Form Factor**: Compact, fits inside skull enclosure
- **Pin Usage**:
  - GPIO 4: Signal output ("GO!" pulse)
  - GPIO 9: BOOT button (factory reset)
  - 3.3V, GND: Power and ground

### 2.2. Signal Output (GPIO 4)
- **Logic Level**: 3.3V HIGH = trigger, 0V LOW = idle
- **Connection**: Direct to animatronic controller GPIO input
- **Cable**: JST-XH or Dupont, max 30cm inside enclosure
- **Protection**: Pull-down resistor on animatronic side recommended
- **Isolation (optional)**: Optocoupler for noisy environments



### 2.4. Power System
- **Input**: USB-C 5V
- **Regulation**: Onboard 3.3V LDO for ESP32 and logic
- **Current Draw**: <500mA typical
- **Future**: LiPo battery + TP4056 charging, voltage divider for battery monitoring

### 2.5. Physical Installation
- **Mounting**: 3M Dual Lock fasteners inside skull
- **Cable Management**: Zip ties, adhesive anchors
- **USB-C Access**: Port accessible for charging/programming


---

## 3. Firmware Design

### 3.1. ESP-IDF & ESP-Matter SDK
- **Framework**: ESP-IDF v5.4.1+
- **Matter SDK**: Latest ESP-Matter release
- **Project Structure**:
  - `main/app_main.cpp`: Main application logic
  - `main/`: Custom drivers, config
  - `sdkconfig`: Project configuration

### 3.2. Matter Device Type & Clusters
- **Device Type**: On/Off Switch (0x0103)
- **Clusters**:
  - Basic Information (mandatory)
  - Identify (mandatory)
  - On/Off (core functionality)
  - Power Source (future: battery reporting)
- **Attributes**:
  - OnOff.OnOff: Boolean (true = GPIO HIGH, false = GPIO LOW)
  - Basic.VendorID: 0xFFF1
  - Basic.ProductID: 0x8001
  - Basic.NodeLabel: "Skull Switch"

### 3.3. GPIO Output Logic
- **Pulse Mode (default)**:
  - On Matter "ON" command: GPIO 4 goes HIGH for configurable duration (default 500ms), then LOW
- On Matter "OFF" command: GPIO 4 goes LOW
- **Toggle Mode (optional)**:
  - GPIO 4 follows Matter switch state (ON = HIGH, OFF = LOW)
- **Debouncing**: 10ms minimum between state changes
- **Timing**: <100ms from Matter command to GPIO change



### 3.5. Factory Reset
- **Method**: Hold BOOT button (GPIO 9) for 5+ seconds
- **Effect**: Clears pairing, Wi-Fi, and resets device

### 3.6. Commissioning
- **QR Code**: Printed to serial monitor on boot
- **Manual Code**: Also printed for fallback
- **Process**: Use Apple Home/Google Home/Alexa to commission

### 3.7. Power Source Cluster (Future)
- **Voltage Divider**: Monitors LiPo battery voltage via ADC
- **Reporting**: Battery % exposed to Matter controller

---

## 4. Animatronic Integration

### 4.1. Signal Protocol
- **Trigger**: Rising edge (LOW to HIGH) on animatronic controller input
- **Recommended**: Use interrupt on animatronic ESP32 for fast response
- **Pulse Duration**: 500ms default, configurable 50ms–5000ms

### 4.2. Example Animatronic Controller Code
```cpp
// ESP32 (Animatronic side)
const int TRIGGER_PIN = 4; // Input from skull switch
volatile bool triggered = false;

void IRAM_ATTR onTrigger() {
    triggered = true;
}

void setup() {
    pinMode(TRIGGER_PIN, INPUT_PULLDOWN);
    attachInterrupt(digitalPinToInterrupt(TRIGGER_PIN), onTrigger, RISING);
}

void loop() {
    if (triggered) {
        triggered = false;
        playRandomAudio();
        // Trigger servos, lights, etc.
    }
}
```

---

## 5. Safety & Reliability
- **Electrical**: Use proper connectors, ensure common ground, protect against reverse polarity
- **EMI**: Shielded cables or optocoupler if needed
- **Thermal**: Ensure airflow, monitor temps in enclosed spaces
- **Testing**: Use multimeter to verify 3.3V signal, test with animatronic controller before final install

---

## 6. Future Enhancements
- **Battery Power**: LiPo + Power Source cluster
- **Multiple GPIO Outputs**: For multi-effect skulls
- **Scene Cluster**: For advanced trigger patterns
- **OTA Updates**: Over-the-air firmware updates
- **Custom PCB**: For compact, robust installation

---

## 7. References
- [spec.md](./spec.md) – Requirements and technical specification
- [circuit_diagram.md](./circuit_diagram.md) – Pinout and wiring
- [skull-integration.md](./skull-integration.md) – Animatronic integration guide
- [ESP32-C3 SuperMini Documentation](https://github.com/sidharthmohannair/Tutorial-ESP32-C3-Super-Mini)
- [ESP-IDF Programming Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c3/index.html)
- [ESP-Matter SDK](https://github.com/espressif/esp-matter) 