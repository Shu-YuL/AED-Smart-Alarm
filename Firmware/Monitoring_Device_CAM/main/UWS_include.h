/* Include Guard */
#ifndef UWS_INCLUDE_H
#define UWS_INCLUDE_H

#include "wifi_include.h"

#ifdef UWS

#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_wpa2.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_netif.h"


#define WIFI_SSID "UWS"
#define WIFI_EAP_ID "uws.ualberta.ca"
#define WIFI_EAP_USERNAME "ccid"
#define WIFI_EAP_PASSWORD "password"

#endif
#endif
