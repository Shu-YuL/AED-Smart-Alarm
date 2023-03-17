/* Include Guard */
#ifndef BUZZER_INCLUDE_H
#define BUZZER_INCLUDE_H

#include <stdio.h>
#include "driver/ledc.h"
#include "esp_err.h"
#include <string.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_http_client.h"

#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
#define LEDC_OUTPUT_IO          (2) // Define the Buzzer output GPIO
#define LEDC_CHANNEL            LEDC_CHANNEL_0
#define LEDC_DUTY_RES           LEDC_TIMER_13_BIT // Set duty resolution to 13 bits
#define LEDC_DUTY               (4095) // Set duty to 50%. ((2 ** 13) - 1) * 50% = 4095
#define LEDC_FREQUENCY          (3000) // Frequency in Hertz.

#define Buzzer_but_PIN 21 // Buzzer control button IO pin

/* function prototypes */
void buzzer_pwm_init(void);
void Buzzer_button_Task(void *params);
void Buzzer_button_config(void);

#endif