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

#define HTTP_GET_URL "https://script.google.com/macros/s/AKfycbygpLjOkEHO2bdSAkp9unIaEdfstvWOnSppsODCJhFMDdWzxixHMzeWGJq5mLeRyz1ttA/exec?read"
#define LOC_MAX_LEN 20

#define Clear_PIN 19 // Clear button IO pin number

/* function prototypes */
void http_get_task(void *pvParameters);
void timer_callback(TimerHandle_t xTimer);
void Clear_Flag_Task(void *params);
void Clear_button(void);
void LCD_Alert(void);
void LCD_Secured(void);

#endif