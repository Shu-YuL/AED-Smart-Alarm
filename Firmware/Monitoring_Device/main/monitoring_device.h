/* Definitions */
/* Change the value below to change external interrupt Pin */
#define Trigger_PIN 18

#define MAC_length 20
#define HTTP_RESPONSE_LEN 10
#define ESP_INR_FLAG_DEFAULT 0

#define HTTP_POST_URL "https://script.google.com/macros/s/AKfycbygpLjOkEHO2bdSAkp9unIaEdfstvWOnSppsODCJhFMDdWzxixHMzeWGJq5mLeRyz1ttA/exec?MAC=%s"

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

/* Function Definitions */
// void send_whatsapp_message(void *arg);
void myMACto_GS(void *parameters);
void get_MAC(void);
// void send_whatup_message(void);
void interrupt_task(void *arg);
void interrupt_init(void);