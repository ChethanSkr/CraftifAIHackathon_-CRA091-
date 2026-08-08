#include "led_controller.h"
#include "app_config.h"
#include "led_strip.h"
#include "led_strip_rmt.h"
#include "esp_check.h"
static led_strip_handle_t strip;
void led_controller_start(void){led_strip_config_t c={.strip_gpio_num=APP_LED_GPIO,.max_leds=APP_LED_COUNT,.led_model=LED_MODEL_WS2812,.color_component_format=LED_STRIP_COLOR_COMPONENT_FMT_GRB};led_strip_rmt_config_t r={.resolution_hz=10*1000*1000};ESP_ERROR_CHECK(led_strip_new_rmt_device(&c,&r,&strip));led_controller_set_state(SENTINEL_STATIC);}
void led_controller_set_state(sentinel_state_t s){if(!strip)return;uint32_t red=0,green=0,blue=0;if(s==SENTINEL_STATIC)green=32;else if(s==SENTINEL_MICRO_MOTION){red=32;green=16;}else red=48;led_strip_set_pixel(strip,0,red,green,blue);led_strip_refresh(strip);}
