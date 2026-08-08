#ifndef CLASSIFIER_H
#define CLASSIFIER_H
#include <stdint.h>

typedef enum { SENTINEL_STATIC, SENTINEL_MICRO_MOTION, SENTINEL_GROSS_MOTION } sentinel_state_t;
typedef struct { float distance_cm; float motion_cm; uint64_t state_age_ms; uint64_t last_alert_ms; sentinel_state_t state; int valid; } sentinel_snapshot_t;
typedef struct { uint64_t timestamp_ms; float distance_cm; sentinel_state_t state; int valid; } sentinel_record_t;

void classifier_start(void);
void classifier_get_snapshot(sentinel_snapshot_t *out);
const char *classifier_state_name(sentinel_state_t state);
uint32_t classifier_get_records(sentinel_record_t *out, uint32_t capacity);
#endif
