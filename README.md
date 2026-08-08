Sentinel — Cabin Guardian

Non-contact micro-motion & presence monitor for vehicle cabin safety, built on ESP32-C3 with FirmGen v0.3.1.

Problem

Children and pets die from heatstroke after being left unattended in parked vehicles — usually accidentally, when a distracted caregiver forgets. Existing mitigations (seat-weight sensors, wearables, checklists) require expensive hardware or active compliance, the exact failure mode they're meant to prevent.

Product Idea

A single ultrasonic sensor continuously classifies activity into three states — STATIC (still), MICRO_MOTION (subtle, e.g. breathing), GROSS_MOTION (rapid movement) — using rolling-window pattern classification, not a single distance threshold. State is shown locally via RGB LED and remotely via a live web dashboard.

Target Users

Parents/caregivers · pet owners who travel · fleet/rideshare/school-transport operators with duty of care.

Generalises To
Domain	Application
Automotive	Cargo tamper/shift monitoring in transit
Automotive	Engine/component vibration health check
Smart home	Breathing/presence monitor (crib, bedside)
Industrial	Equipment idle/running/fault classification
Healthcare	Fall or prolonged-inactivity detection

Same STATIC/MICRO_MOTION/GROSS_MOTION core; only thresholds and interpretation change per use case.

Bill of Materials

ESP32-C3 dev board · HC-SR04 ultrasonic sensor · 1kΩ + 2kΩ resistors (voltage divider) · breadboard · jumper wires · USB cable.

Wiring
HC-SR04	Connects To	Notes
VCC	ESP32 5V	Needs full 5V
GND	ESP32 GND	Common ground required
TRIG	GPIO 4	Direct — 3.3V compatible
ECHO	GPIO 5 (via divider)	5V signal 


Build & Run
Install ESP-IDF, verify in FirmGen's Toolchain Status.
Open existing project in FirmGen, confirm board = ESP32-C3.
Set Wi-Fi SSID/password in app_config.h.
Review Task List / Firmware Topology, then Deploy → Build, flash & monitor.
Read device IP from serial monitor, open it in a browser (same Wi-Fi) for the live dashboard.
Source Layout
sensor_driver/ — triggers HC-SR04, times echo, handles timeouts
classifier/ — rolling window, variance/delta, debounce/cooldown → state
led_controller/ — RGB feedback
web_server/ — dashboard, JSON telemetry, live threshold tuning
app_config.h — all thresholds, timing, Wi-Fi config, centralized


Limitations
No HTTPS → browser camera capture blocked on mobile
Thresholds tuned for dev environment; may need retuning (exposed via dashboard sliders)
Single-point line-of-sight sensing — detects motion pattern, not identity
No MQTT/cloud persistence in this build
Dashboard needs Wi-Fi; local LED feedback works offline
Dev Process

Built incrementally, each layer verified on hardware before the next: LED blink → RGB control → Web LED control → Ultrasonic + classification → Dashboard chart/log/thresholds → Themed UI.

TASKS List
<img width="1917" height="1015" alt="Screenshot 2026-08-08 161044" src="https://github.com/user-attachments/assets/a275db9b-bc94-4910-9ca5-11da2ff50e3b" />

Working Video
https://github.com/user-attachments/assets/b2b43c0a-bc94-4a77-a17b-8c4668370b4c

Final Output Screen
<img width="1917" height="967" alt="image" src="https://github.com/user-attachments/assets/68b14fae-b000-44ad-b366-b2461f43d026" />
