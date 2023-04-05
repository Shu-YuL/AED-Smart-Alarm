/* Include Guard */
#ifndef MONITORING_DEVICE_H
#define MONITORING_DEVICE_H

/* Includes */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_system.h"
#include "esp_http_client.h"
#include "driver/gpio.h"
#include "esp_mac.h"
#include "esp_wifi.h"

#include "esp_sleep.h"
#include "driver/rtc_io.h"

/* Definitions */
#define MAC_length 20
#define HTTP_RESPONSE_LEN 10
#define ESP_INR_FLAG_DEFAULT 0
/* Test URL to perform HTTP request */
#define HTTP_GET_URL "https://script.google.com/macros/s/AKfycbw_xZIRQbZ2CVw4Qh_JAPOqhXy6XPgs0BYqGGMt2zvEElzEetMWkGOyVf_LmU3XIHQ9Ug/exec?MAC=%s"
/* Official URL to perform HTTP request */
// #define HTTP_GET_URL "https://script.google.com/macros/s/AKfycbz00T8xuK0Al_jOZzJMmi17tDMMRb6GcCaSH8MJ2rD32jHkeRXG0UCwRyipQSP2xmRs/exec?MAC=%s"

#define WAKEUP_PIN 25

extern char my_MAC[MAC_length];

/* Function Prototypes */
void myMACto_GS(void *parameters);
void get_MAC(void);
void configure_sleep(void);
void enter_deep_sleep(void);

#endif