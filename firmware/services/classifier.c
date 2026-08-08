#include "classifier.h"
#include "sensor_driver.h"
#include "sentinel_config.h"
#include "app_config.h"
#include "led_controller.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include <math.h>

static sentinel_snapshot_t snap={.state=SENTINEL_STATIC};
static float win[APP_WINDOW_SIZE];
static sentinel_record_t records[APP_HISTORY_SIZE];
static uint32_t record_count;
static int count,pos,confirm;
static sentinel_state_t candidate;
static portMUX_TYPE mux=portMUX_INITIALIZER_UNLOCKED;

const char *classifier_state_name(sentinel_state_t s){return s==SENTINEL_GROSS_MOTION?"GROSS_MOTION":s==SENTINEL_MICRO_MOTION?"MICRO_MOTION":"STATIC";}
void classifier_get_snapshot(sentinel_snapshot_t *o){if(!o)return;portENTER_CRITICAL(&mux);*o=snap;portEXIT_CRITICAL(&mux);}
uint32_t classifier_get_records(sentinel_record_t *out,uint32_t capacity){if(!out||capacity==0)return 0;portENTER_CRITICAL(&mux);uint32_t n=record_count<capacity?record_count:capacity;uint32_t start=record_count>APP_HISTORY_SIZE?record_count-APP_HISTORY_SIZE:0;for(uint32_t i=0;i<n;i++)out[i]=records[(start+i)%APP_HISTORY_SIZE];portEXIT_CRITICAL(&mux);return n;}

static void task(void *arg){(void)arg;sensor_reading_t r;for(;;){if(!sensor_driver_receive(&r,APP_SAMPLE_PERIOD_MS*2))continue;if(!r.valid){portENTER_CRITICAL(&mux);snap.valid=0;records[record_count%APP_HISTORY_SIZE]=(sentinel_record_t){r.timestamp_ms,0,snap.state,0};record_count++;portEXIT_CRITICAL(&mux);continue;}win[pos]=r.distance_cm;pos=(pos+1)%APP_WINDOW_SIZE;if(count<APP_WINDOW_SIZE)count++;float min=win[0],max=win[0],step=0;for(int i=1;i<count;i++){if(win[i]<min)min=win[i];if(win[i]>max)max=win[i];float d=fabsf(win[i]-win[i-1]);if(d>step)step=d;}float motion=max-min;sentinel_runtime_config_t c;sentinel_config_get(&c);sentinel_state_t next=(motion>=c.motion_threshold_cm*APP_GROSS_MOTION_MULTIPLIER||step>=c.motion_threshold_cm*APP_GROSS_MOTION_MULTIPLIER)?SENTINEL_GROSS_MOTION:(motion>=c.motion_threshold_cm?SENTINEL_MICRO_MOTION:SENTINEL_STATIC);portENTER_CRITICAL(&mux);sentinel_state_t current=snap.state;portEXIT_CRITICAL(&mux);if(next!=current){if(next==candidate)confirm++;else{candidate=next;confirm=1;}if(confirm>=APP_CONFIRMATION_SAMPLES){portENTER_CRITICAL(&mux);snap.state=next;snap.state_age_ms=r.timestamp_ms;snap.last_alert_ms=next==SENTINEL_STATIC?snap.last_alert_ms:r.timestamp_ms;portEXIT_CRITICAL(&mux);led_controller_set_state(next);confirm=0;}}else confirm=0;portENTER_CRITICAL(&mux);snap.distance_cm=r.distance_cm;snap.motion_cm=motion;snap.valid=1;snap.state_age_ms=r.timestamp_ms-snap.state_age_ms;records[record_count%APP_HISTORY_SIZE]=(sentinel_record_t){r.timestamp_ms,r.distance_cm,snap.state,1};record_count++;portEXIT_CRITICAL(&mux);}}
void classifier_start(void){xTaskCreate(task,"sentinel_classifier",4096,NULL,7,NULL);}
