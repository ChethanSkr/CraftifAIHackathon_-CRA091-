Project : Sentinel — Cabin Guardian

Sentinel — Cabin Guardian
Non-contact micro-motion & presence monitor for vehicle cabin safety, built on ESP32-C3 with FirmGen v0.3.1

Problem

Children/pets die from heatstroke when left unattended in parked vehicles — usually accidental
Existing fixes (seat-weight sensors, wearables, checklists) need costly hardware or caregiver compliance — the exact failure mode they're meant to prevent

Product Idea

Single ultrasonic sensor classifies activity into three states via rolling-window pattern classification (not a single threshold):
STATIC — still
MICRO_MOTION — subtle, e.g. breathing
GROSS_MOTION — rapid movement
State shown locally via RGB LED and remotely via live web dashboard

Target Users

Parents/caregivers of young children
Pet owners who travel
Fleet, rideshare, school-transport operators

Generalizes To (same 3-state core, only thresholds change)

Automotive — cargo tamper/shift monitoring
Automotive — engine/component vibration health
Smart home — breathing/presence monitor (crib, bedside)
Industrial — equipment idle/running/fault classification
Healthcare — fall or prolonged-inactivity detection

Bill of Materials

ESP32-C3 dev board
HC-SR04 ultrasonic sensor
1kΩ + 2kΩ resistors (voltage divider)
Breadboard, jumper wires, USB cable

Wiring

VCC → ESP32 5V
GND → ESP32 GND (common ground required)
TRIG → GPIO 4
ECHO → GPIO 5 via divider 


Build & Run

1. Install ESP-IDF, verify in FirmGen Toolchain Status
2. Open project in FirmGen, confirm board = ESP32-C3
3. Set Wi-Fi SSID/password in app_config.h
4. Review Task List/Firmware Topology, then Deploy → Build, flash & monitor
5. Read device IP from serial monitor, open in browser (same Wi-Fi) for dashboard

Source Layout

sensor_driver/ — triggers HC-SR04, times echo, handles timeouts
classifier/ — rolling window, variance/delta, debounce/cooldown → state
led_controller/ — RGB feedback
web_server/ — dashboard, JSON telemetry, live threshold tuning
app_config.h — centralized thresholds, timing, Wi-Fi config

Limitations

1. No HTTPS → camera capture blocked on mobile browsers
2. Thresholds tuned for dev environment; may need retuning (dashboard sliders exposed for this)
3. Single-point, line-of-sight sensing — detects pattern, not identity
4. No MQTT/cloud persistence in this build
5. Dashboard needs Wi-Fi; local LED feedback works offline

Dev Process

Built incrementally, each layer verified on hardware before the next: LED blink → RGB control → Web LED control → Ultrasonic + classification → Dashboard chart/log/thresholds → Themed UI

TASKS List
<img width="1917" height="1015" alt="Screenshot 2026-08-08 161044" src="https://github.com/user-attachments/assets/a275db9b-bc94-4910-9ca5-11da2ff50e3b" />

Working Video
https://github.com/user-attachments/assets/b2b43c0a-bc94-4a77-a17b-8c4668370b4c

Final Output Screen
<img width="1917" height="967" alt="image" src="https://github.com/user-attachments/assets/68b14fae-b000-44ad-b366-b2461f43d026" />
