/* Include Guard */
#ifndef BASE_STATION_H
#define BASE_STATION_H

#include <stdio.h>
#include <string.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_http_client.h"
#include "esp_mac.h"

#define TIMER_PERIOD_MS 5000 // timer counting duration
#define TASK_STACK_SIZE 8192 // HTTP request task stack size

/* Test URL to perform HTTP request */
#define HTTP_GET_URL "https://script.google.com/macros/s/AKfycbw_xZIRQbZ2CVw4Qh_JAPOqhXy6XPgs0BYqGGMt2zvEElzEetMWkGOyVf_LmU3XIHQ9Ug/exec?read"
/* Official URL to perform HTTP request */
// #define HTTP_GET_URL "https://script.google.com/macros/s/AKfycbz00T8xuK0Al_jOZzJMmi17tDMMRb6GcCaSH8MJ2rD32jHkeRXG0UCwRyipQSP2xmRs/exec?read"

#define LOC_MAX_LEN 20

#define Clear_PIN 22 // Clear button IO pin number 22

/* function prototypes */
void http_get_task(void *pvParameters);
void timer_callback(TimerHandle_t xTimer);
void Clear_Flag_Task(void *params);
void Clear_button(void);
void LCD_Alert(void);
void LCD_Secured(void);

#endif