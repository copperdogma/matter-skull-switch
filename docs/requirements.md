## 1. Functional Requirements

*   **FR1: Occupancy Detection:** The primary function is to detect human presence using an HC-SR501 PIR (Passive Infrared) sensor.
*   **FR2: Matter Connectivity:** The device must implement the Matter protocol to connect and communicate with compatible smart home ecosystems (e.g., Apple HomeKit, Google Home, Amazon Alexa).
    *   **FR2.1:** The device shall expose an occupancy sensing cluster as defined by the Matter specification.
*   **FR3: Wi-Fi Communication:** The device will use Wi-Fi for network communication.
*   **FR4: Configuration:** Basic configuration (e.g., Wi-Fi credentials, Matter commissioning) should be possible through standard Matter procedures.
*   **FR5: Power Supply:** The device will be powered via USB.

## 2. Non-Functional Requirements

*   **NFR1: Hardware Platform:** The device will be built using an ESP32-C3 SuperMini microcontroller.
    *   **Reference:** [https://github.com/sidharthmohannair/Tutorial-ESP32-C3-Super-Mini](https://github.com/sidharthmohannair/Tutorial-ESP32-C3-Super-Mini)
*   **NFR2: Firmware:** The firmware will be developed using the ESP-IDF (Espressif IoT Development Framework) and the ESP-Matter SDK.
*   **NFR3: Reliability:** The device should operate reliably with minimal downtime.
*   **NFR4: Responsiveness:** Occupancy detection and status reporting via Matter should be timely (e.g., within 1-2 seconds of detection).
*   **NFR5: Security:** Communication must be secured as per Matter protocol standards.
*   **NFR6: Low Power (Consideration):** While the primary power source is USB, considerations for power efficiency in the firmware design are encouraged for potential future battery-powered versions (though not a strict requirement for this version).

## 3. Environmental Requirements

*   **ER1: Operating Temperature:** Standard indoor room temperatures (e.g., 10°C to 35°C).
*   **ER2: Humidity:** Standard indoor humidity levels.

## 4. Sensor Specifics (HC-SR501)

*   **SR1: Detection Range:** ~3-7 meters (configurable via potentiometer on sensor).
*   **SR2: Detection Angle:** ~100-120 degrees.
*   **SR3: Output:** Digital HIGH when occupancy detected, LOW otherwise.
*   **SR4: Trigger Modes:** Repeatable (H) or Non-repeatable (L) trigger (configurable via jumper on sensor). Default is typically repeatable.

## 5. Future Considerations (Out of Scope for Initial Version)

*   **FC1:** OTA (Over-The-Air) firmware updates.
*   **FC2:** Low-power sleep modes for battery operation.
*   **FC3:** Adjustable sensitivity and delay via Matter. 