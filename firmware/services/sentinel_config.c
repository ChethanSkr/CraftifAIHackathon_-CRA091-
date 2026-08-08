#include "sentinel_config.h"
#include "app_config.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include <math.h>
static SemaphoreHandle_t lock; static sentinel_runtime_config_t cfg;
void sentinel_config_init(void){ lock=xSemaphoreCreateMutex(); cfg=(sentinel_runtime_config_t){APP_MOTION_THRESHOLD_CM,APP_ALERT_COOLDOWN_SECONDS}; }
void sentinel_config_get(sentinel_runtime_config_t *out){ if(!out)return; xSemaphoreTake(lock,portMAX_DELAY); *out=cfg; xSemaphoreGive(lock); }
int sentinel_config_update(float t,uint32_t c){ if(!isfinite(t)||t<0.2f||t>100.0f||c>3600)return 0; xSemaphoreTake(lock,portMAX_DELAY); cfg.motion_threshold_cm=t; cfg.cooldown_seconds=c; xSemaphoreGive(lock); return 1; }
