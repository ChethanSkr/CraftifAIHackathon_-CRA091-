#include "sensor_driver.h"
#include "app_config.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_log.h"
#include <stdbool.h>
static const char *TAG="sensor"; static QueueHandle_t queue;
static bool wait_level(int level, int64_t deadline){ while(gpio_get_level(APP_ECHO_GPIO)!=level){ if(esp_timer_get_time()>=deadline)return false; } return true; }
static void task(void *arg){ (void)arg; gpio_config_t out={.pin_bit_mask=1ULL<<APP_TRIG_GPIO,.mode=GPIO_MODE_OUTPUT}; gpio_config(&out); gpio_set_level(APP_TRIG_GPIO,0); gpio_config_t in={.pin_bit_mask=1ULL<<APP_ECHO_GPIO,.mode=GPIO_MODE_INPUT,.pull_down_en=GPIO_PULLDOWN_DISABLE,.pull_up_en=GPIO_PULLUP_DISABLE}; gpio_config(&in); for(;;){ gpio_set_level(APP_TRIG_GPIO,1); esp_rom_delay_us(10); gpio_set_level(APP_TRIG_GPIO,0); int64_t start=esp_timer_get_time(), rise, fall; sensor_reading_t r={0}; if(wait_level(1,start+APP_ECHO_TIMEOUT_US) && (rise=esp_timer_get_time()) && wait_level(0,rise+APP_ECHO_TIMEOUT_US)){ fall=esp_timer_get_time(); r.distance_cm=(float)(fall-rise)*0.0343f/2.0f; r.valid=r.distance_cm>2.0f && r.distance_cm<450.0f; } r.timestamp_ms=(uint64_t)(esp_timer_get_time()/1000); if(xQueueOverwrite(queue,&r)!=pdPASS) ESP_LOGW(TAG,"reading queue unavailable"); vTaskDelay(pdMS_TO_TICKS(APP_SAMPLE_PERIOD_MS)); } }
void sensor_driver_start(void){ queue=xQueueCreate(1,sizeof(sensor_reading_t)); xTaskCreate(task,"sentinel_sensor",3072,NULL,8,NULL); }
int sensor_driver_receive(sensor_reading_t *reading,uint32_t timeout_ms){ return queue && xQueueReceive(queue,reading,pdMS_TO_TICKS(timeout_ms))==pdPASS; }
