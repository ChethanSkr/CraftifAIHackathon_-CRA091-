#include "app.h"
#include "app_config.h"
#include "logger.h"
#include "sentinel_config.h"
#include "sensor_driver.h"
#include "classifier.h"
#include "led_controller.h"
#include "wifi_service.h"
#include "web_server.h"

static const char *TAG = "sentinel";
void app_start(void)
{
    ESP_LOGI(TAG, "Sentinel starting: TRIG GPIO%d, ECHO GPIO%d, LED GPIO%d", APP_TRIG_GPIO, APP_ECHO_GPIO, APP_LED_GPIO);
    sentinel_config_init();
    led_controller_start();
    sensor_driver_start();
    classifier_start();
    wifi_service_start();
    web_server_start();
    ESP_LOGI(TAG, "HC-SR04 ECHO must use a 5 V to 3.3 V voltage divider");
}
