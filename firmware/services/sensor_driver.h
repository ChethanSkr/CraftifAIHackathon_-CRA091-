#ifndef SENSOR_DRIVER_H
#define SENSOR_DRIVER_H
#include <stdint.h>
typedef struct { float distance_cm; int valid; uint64_t timestamp_ms; } sensor_reading_t;
void sensor_driver_start(void);
int sensor_driver_receive(sensor_reading_t *reading, uint32_t timeout_ms);
#endif
