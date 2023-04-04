#include "led.h"

/* Description: 
If an alarm is triggered, display flashing red on the alarm LED. Make sure turn_off_green_LED() is called, and then flash_red_LED().
If user presses the clear button, display a solid red on the alarm LED. Use turn_on_red_LED().
When there are no active alarms, display green on the alarm LED. Makre sure turn_off_red_LED() is called, and then turn_on_green_LED().
*/

//NOTE: The timer number 'LED_LEDC_TIMER' and channel number 'LED_LEDC_CHANNEL' must be different from that of the buzzer, which also uses PWM using LEDC


/*------------------------------------------
Function Name: flash_red_LED()
Description: Displays a flashing red light on the alarm LED. 
             Use this function to indicate a new triggered alarm in the Spreadsheet that has not been acknowledged by the user.
Input: N/A
Output: N/A
Variables Affected: N/A
-------------------------------------------*/
void flash_red_LED(void) {

    //configure alarm LED settings
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = RED_LED_LEDC_MODE,
        .timer_num        = RED_LED_LEDC_TIMER,
        .duty_resolution  = RED_LED_LEDC_DUTY_RES,
        .freq_hz          = RED_LED_LEDC_FREQUENCY,  // Set output frequency
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    ledc_channel_config_t ledc_channel = {
        .speed_mode     = RED_LED_LEDC_MODE,
        .channel        = RED_LED_LEDC_CHANNEL,
        .timer_sel      = RED_LED_LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = RED_LED_PIN,
        .duty           = 0, // initialize duty cycle to 0%
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));

    //set duty cycle to 50%
    ESP_ERROR_CHECK(ledc_set_duty(RED_LED_LEDC_MODE, RED_LED_LEDC_CHANNEL, RED_LED_LEDC_DUTY));

    //update duty cycle to apply the new value
    ESP_ERROR_CHECK(ledc_update_duty(RED_LED_LEDC_MODE, RED_LED_LEDC_CHANNEL));
}


/*------------------------------------------
Function Name: turn_on_red_LED()
Description: Displays solid red light on the alarm LED. Will also disable flashing red. A solid red is meant to indicate that there is still 
             an active alarm present in the Spreadsheet, but the user has acknowledged it by sending a clear alarm request. 
             Call this function when the clear button on the base station is pressed.
Input: N/A
Output: N/A
Variables Affected: N/A
-------------------------------------------*/
void turn_on_red_LED(void) {
    ledc_stop(RED_LED_LEDC_MODE, RED_LED_LEDC_CHANNEL, 1);
}


/*------------------------------------------
Function Name: turn_off_red_LED()
Description: Turns off red on the alarm LED. Will also disable flashing red.
Input: N/A
Output: N/A
Variables Affected: N/A
-------------------------------------------*/
void turn_off_red_LED(void) {
    ledc_stop(RED_LED_LEDC_MODE, RED_LED_LEDC_CHANNEL, 0);
}


/*------------------------------------------
Function Name: turn_on_green_LED()
Description: Displays a solid green light on the alarm LED. A solid green is meant to indicate that there are no triggered alarms. 
             Call this function when the Spreadsheet is empty.
Input: N/A
Output: N/A
Variables Affected: N/A
-------------------------------------------*/
void turn_on_green_LED(void) {
    gpio_set_direction(GREEN_LED_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(GREEN_LED_PIN, 1);
}


/*------------------------------------------
Function Name: turn_off_green_LED()
Description: Turns off green on the alarm LED.
Input: N/A
Output: N/A
Variables Affected: N/A
-------------------------------------------*/
void turn_off_green_LED(void) {
    gpio_set_direction(GREEN_LED_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(GREEN_LED_PIN, 0);
}