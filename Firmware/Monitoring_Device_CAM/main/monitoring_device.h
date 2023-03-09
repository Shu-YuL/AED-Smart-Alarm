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

/* Definitions */
/* Change the value below to change external interrupt Pin */
#define Trigger_PIN 18

#define MAC_length 20
#define IP_length 15
#define HTTP_RESPONSE_LEN 10
#define ESP_INR_FLAG_DEFAULT 0

#define HTTP_GET_URL "https://script.google.com/macros/s/AKfycbwaTvT9aut2pFeq2BCMkPpkYDqu8bX7S-qF6RX0l2_YtwOzVQx3EEnIrhQKvNR2C8eQ-A/exec?MAC=%s&IP=%s"

extern char my_MAC[MAC_length];
extern char my_IP[IP_length];;

/* Function Definitions */
void myMACto_GS(void *parameters);
void get_MAC(void);
void interrupt_task(void *arg);
void interrupt_init(void);

#endif