#ifndef SERVER_INCLUDE_H
#define SERVER_INCLUDE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h> //Requires by memset
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include <esp_http_server.h>

#include "esp_wifi.h"
#include "esp_event.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "driver/gpio.h"
#include <lwip/sockets.h>
#include <lwip/sys.h>
#include <lwip/api.h>
#include <lwip/netdb.h>

#include "wifi_include.h"
#include "ssd1306.h"
#include "font8x8_basic.h"

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

#define MAC_LENGTH 18
#define BUD_LENGTH 6
#define FR_LENGTH 2

#define MAX_INPUTS 10
#define INPUT_LENGTH 50
#define ID_LENGTH 50

#define LED_PIN 2

#define EXAMPLE_ESP_WIFI_SSID WIFI_SSID
#define EXAMPLE_ESP_WIFI_PASS WIFI_PASSWORD
#define EXAMPLE_ESP_MAXIMUM_RETRY WIFI_MAX_TRIES

/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1

/* function prototypes */
httpd_handle_t setup_server(void);
// void stop_webserver(httpd_handle_t server);
// void restart_webserver(httpd_handle_t server);

/* Global variables */
extern char mac_array[100][MAC_LENGTH];
extern char building_array[100][BUD_LENGTH];
extern char floor_array[100][FR_LENGTH];

extern const char main_temp[];
extern int num_rows;

extern SSD1306_t dev; // oled device address global variable from main

#endif