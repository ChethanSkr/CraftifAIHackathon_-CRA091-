#ifndef SENTINEL_CONFIG_H
#define SENTINEL_CONFIG_H
#include <stdint.h>
typedef struct { float motion_threshold_cm; uint32_t cooldown_seconds; } sentinel_runtime_config_t;
void sentinel_config_init(void);
void sentinel_config_get(sentinel_runtime_config_t *out);
int sentinel_config_update(float threshold_cm, uint32_t cooldown_seconds);
#endif
