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
// #define HTTP_GET_URL "https://script.google.com/macros/s/AKfycbyep2FqLkWTKd59XXp0cULkYX_-l1KyVdksIugKmTktqbAtWeEz-6xSAh9aoOt9Pw4MQw/exec?read"
/* Official URL to perform HTTP request */
#define HTTP_GET_URL "https://script.google.com/macros/s/AKfycbyJ4tUr6knxRxIuVFZdto3vQio3abIqe8loPH8j4ymoyhJoLbtOZMy6TNae-TfGmGWH/exec?read"

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