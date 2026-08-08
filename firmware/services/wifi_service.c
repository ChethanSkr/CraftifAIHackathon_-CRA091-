#include "wifi_service.h"
#include "app_config.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include <string.h>
static const char *TAG="wifi";
static void on_event(void* a,esp_event_base_t b,int32_t id,void*d){(void)a;(void)d;if(b==WIFI_EVENT&&id==WIFI_EVENT_STA_START)esp_wifi_connect();if(b==WIFI_EVENT&&id==WIFI_EVENT_STA_DISCONNECTED){ESP_LOGW(TAG,"Wi-Fi disconnected; retrying");esp_wifi_connect();}if(b==IP_EVENT&&id==IP_EVENT_STA_GOT_IP)ESP_LOGI(TAG,"Dashboard network ready");}
void wifi_service_start(void){esp_err_t n=nvs_flash_init();if(n==ESP_ERR_NVS_NO_FREE_PAGES||n==ESP_ERR_NVS_NEW_VERSION_FOUND){nvs_flash_erase();nvs_flash_init();}ESP_ERROR_CHECK(esp_netif_init());esp_err_t e=esp_event_loop_create_default();if(e!=ESP_OK&&e!=ESP_ERR_INVALID_STATE)ESP_ERROR_CHECK(e);esp_netif_create_default_wifi_sta();wifi_init_config_t w=WIFI_INIT_CONFIG_DEFAULT();ESP_ERROR_CHECK(esp_wifi_init(&w));ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT,ESP_EVENT_ANY_ID,on_event,NULL));ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT,IP_EVENT_STA_GOT_IP,on_event,NULL));wifi_config_t c={};strncpy((char*)c.sta.ssid,APP_WIFI_SSID,sizeof(c.sta.ssid));strncpy((char*)c.sta.password,APP_WIFI_PASSWORD,sizeof(c.sta.password));c.sta.threshold.authmode=WIFI_AUTH_WPA2_PSK;ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA,&c));ESP_ERROR_CHECK(esp_wifi_start());}
