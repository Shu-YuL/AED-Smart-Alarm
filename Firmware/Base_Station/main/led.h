/* Include Guard */
#ifndef LED_H
#define LED_H

#include <stdio.h>
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_err.h"

#define RED_LED_PIN                 33 // GPIO number of red pin of RGB Alarm LED
#define GREEN_LED_PIN               25 // GPIO number of green pin of RGB Alarm LED

#define RED_LED_LEDC_TIMER          LEDC_TIMER_1
#define RED_LED_LEDC_MODE           LEDC_LOW_SPEED_MODE
#define RED_LED_LEDC_CHANNEL        LEDC_CHANNEL_1
#define RED_LED_LEDC_DUTY_RES       LEDC_TIMER_13_BIT // Set duty resolution to 13 bits
#define RED_LED_LEDC_DUTY           4095 // Set duty to 50%. ((2 ** 13) - 1) * 50% = 4095
#define RED_LED_LEDC_FREQUENCY      1 // Frequency in Hertz of flashing red LED when an alarm is active

/* Alarm LED functions */
void flash_red_LED(void);
void turn_on_red_LED(void);
void turn_off_red_LED(void);
void turn_on_green_LED(void);
void turn_off_green_LED(void);

#endif