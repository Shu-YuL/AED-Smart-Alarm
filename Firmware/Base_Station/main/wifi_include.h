#ifndef WIFI_INCLUDE_H
#define WIFI_INCLUDE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "driver/gpio.h"
#include "esp_wifi.h"
#include "esp_wpa2.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_netif.h"

#define WIFI_MODE 0
#define WIFI_SSID "" //enter network ssid
#define WIFI_PASSWORD "" //enter network password

#define WIFI_EAP_METHOD 0 //following is to be used once working WPA2 example is integrated 
#define WIFI_EAP_ID 0
#define WIFI_EAP_USERNAME 0
#define WIFI_EAP_PASSWORD 0
#define WIFI_MAX_TRIES 10
#define WIFI_AUTHORIZATION_MODE_THRESHOLD  WIFI_AUTH_OPEN //The weakest authmode allowed of the AP

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1

/* function prototypes */
void setup_nvs(void);
void connect_wifi(void);

#endif