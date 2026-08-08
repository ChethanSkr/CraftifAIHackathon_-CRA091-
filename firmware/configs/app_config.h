#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#define APP_TRIG_GPIO 4
#define APP_ECHO_GPIO 5
#define APP_LED_GPIO 8
#define APP_LED_COUNT 1
#define APP_WIFI_SSID "realme narzo 60 Pro 5G"
#define APP_WIFI_PASSWORD "12345678"
/* HC-SR04 measurements are recorded every 50 ms for responsive logging.
   Keep the sensor physically isolated from nearby ultrasonic sources. */
#define APP_SAMPLE_PERIOD_MS 50
#define APP_ECHO_TIMEOUT_US 25000
#define APP_WINDOW_SIZE 20
#define APP_CONFIRMATION_SAMPLES 5
#define APP_MOTION_THRESHOLD_CM 1.5f
#define APP_GROSS_MOTION_MULTIPLIER 4.0f
#define APP_ALERT_COOLDOWN_SECONDS 5
#define APP_HISTORY_SIZE 240
#define APP_EVENT_SIZE 20

#endif
