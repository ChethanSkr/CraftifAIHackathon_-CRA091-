# Sentinel — Cabin Guardian

Sentinel is an assistive ESP32-C3 vehicle-cabin monitor. It samples an HC-SR04 ultrasonic sensor, classifies STATIC, MICRO_MOTION, and GROSS_MOTION states, drives the onboard RGB indicator, and serves a lightweight dashboard over Wi-Fi.

## Wiring

- HC-SR04 TRIG: GPIO4
- HC-SR04 ECHO: GPIO5 through a voltage divider
- Onboard RGB LED: GPIO8 using `led_strip`
- Shared ground required

HC-SR04 ECHO may be 5 V. Never connect it directly to GPIO5. Use a divider such as 10 kΩ from ECHO to GPIO5 and 20 kΩ from GPIO5 to ground, and verify the resulting voltage before connection.

## Configuration and operation

Replace the Wi-Fi placeholders in `firmware/configs/app_config.h`, build, and flash. Browse to the device IP address after Wi-Fi connects. Runtime motion settings are intended to be exposed through the dashboard API. This prototype is not a certified life-safety system.
