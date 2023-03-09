#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_intr_alloc.h"

#define piezoEN GPIO_NUM_2 
#define ledB1 GPIO_NUM_13 
#define ledG1 GPIO_NUM_14
#define ledR1 GPIO_NUM_27
#define ledB0 GPIO_NUM_26
#define ledG0 GPIO_NUM_25
#define ledR0 GPIO_NUM_33
#define clrAlmSW GPIO_NUM_22
#define piezoRstSw GPIO_NUM_21

static uint8_t defRed[] = [1, 0, 0]
static uint8_t defGreen[] = [0, 1, 0]
static uint8_t defBlue[] = [0, 0, 1]
static uint8_t defAmber[] = [1, 1, 0]

static void configAlmLED(void){
    // Initializing LEDO (Alarm Status LED)
    gpio_set_direction(ledR0, GPIO_MODE_OUTPUT);
    gpio_set_direction(ledG0, GPIO_MODE_OUTPUT);
    gpio_set_direction(ledB0, GPIO_MODE_OUTPUT);
}

static void configWifiLED(void){
    // Initializing LED1 (Wifi Connection Status)
    gpio_set_direction(ledR1, GPIO_MODE_OUTPUT);
    gpio_set_direction(ledG1, GPIO_MODE_OUTPUT);
    gpio_set_direction(ledB1, GPIO_MODE_OUTPUT);
}

static void configSW_Periph(void){
    // Initializing Piezo Buzzer, Clear Alarm Switch, Piezo Reset Switch
    gpio_set_direction(piezoEN, GPIO_MODE_OUTPUT);
    gpio_set_direction(clrAlmSW, GPIO_MODE_INPUT);
    gpio_set_direction(piezoRstSw, GPIO_MODE_INPUT);
}

static void setLedColor(char color, esp_err_t red, esp_err_t green, esp_err_t blue){
    if (color == 'r'){
        gpio_set_level(red, defRed[0]);
        gpio_set_level(green, defRed[1]);
        gpio_set_level(blue, defRed[2]);
    }
    else if (color == 'b'){
        gpio_set_level(red, defBlue[0]);
        gpio_set_level(green, defBlue[1]);
        gpio_set_level(blue, defBlue[2]);
    }
    else if (color == 'a'){
        gpio_set_level(red, defAmber[0]);
        gpio_set_level(green,defAmber[1]);
        gpio_set_level(blue, defAmber[2]);              
    }
    else{
        gpio_set_level(red, defGreen[0]);
        gpio_set_level(green, defGreen[1]);
        gpio_set_level(blue, defGreen[2]); 
    }
}

static void ledStrobe(char strobeColor, uint8_t ledIdent){
    gpio_num_t pinR;
    gpio_num_t pinG;
    gpio_num_t pinB;
    esp_err_t red;
    esp_err_t green;
    esp_err_t blue;

    if (ledIdent == 0){
        pinR = (gpio_num_t)(ledR0 & 0x1F);
        pinG = (gpio_num_t)(ledG0 & 0x1F);
        pinB = (gpio_num_t)(ledB0 & 0x1F);
        red = ledR0;
        green = ledG0;
        blue = ledB0;
    }
    else{
        pinR = (gpio_num_t)(ledR1 & 0x1F);
        pinG = (gpio_num_t)(ledG1 & 0x1F);
        pinB = (gpio_num_t)(ledB1 & 0x1F);
        red = ledR1;
        green = ledG1;
        blue = ledB1;
    }
    
    while (strobeLed){
        uint8_t sRed = (GPIO_REG_READ(GPIO_OUT_REG) >> pinR) & 1U;
        uint8_t sGreen = (GPIO_REG_READ(GPIO_OUT_REG) >> pinG) & 1U;
        uint8_t sBlue = (GPIO_REG_READ(GPIO_OUT_REG) >> pinB) & 1U;

        if (strobeColor == 'g'){
            gpio_set_level(green, !sGreen);
        }
        else if (strobeColor == 'b'){
            gpio_set_level(blue, !sBlue);
        }
        else if (strobeColor == 'a'){
            gpio_set_level(red, !sRed);
            gpio_set_level(green, !sGreen);             
        }
        else{
            gpio_set_level(red, !sRed);
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

}

static void setAlrmLED(uint8_t led0State){
    switch (led0State)
    {
        case 0: // Case 0 = Red
            setLedColor('r', ledR0, ledG0, ledB0);
        case 1: // Case 1 = Amber
            setLedColor('a', ledR0, ledG0, ledB0);
        default: // Default = Green
            setLedColor('g', ledR0, ledG0, ledB0);         
    }
}

static void setBatLED(int led1State){
    switch (led1State)
    {
        case 0: // Case 0 = Red
            setLedColor('r', ledR1, ledG1, ledB1);
        case 1: // Case 1 = Amber
            setLedColor('a', ledR1, ledG1, ledB1);
        default: // Default = Blue
            setLedColor('b', ledR1, ledG1, ledB1);
    }
}

void app_main(void){
    configAlmLED();
    configWifiLED();
    configSW_Periph();

    uint8_t alarmState = 0;

}
// Buzzer Reset Interrupt


// /* Blink Example

//    This example code is in the Public Domain (or CC0 licensed, at your option.)

//    Unless required by applicable law or agreed to in writing, this
//    software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
//    CONDITIONS OF ANY KIND, either express or implied.
// */
// #include <stdio.h>
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "driver/gpio.h"
// #include "esp_log.h"
// #include "led_strip.h"
// #include "sdkconfig.h"

// static const char *TAG = "example";

// /* Use project configuration menu (idf.py menuconfig) to choose the GPIO to blink,
//    or you can edit the following line and set a number here.
// */
// #define BLINK_GPIO CONFIG_BLINK_GPIO

// static uint8_t s_led_state = 0;

// #ifdef CONFIG_BLINK_LED_RMT

// static led_strip_handle_t led_strip;

// static void blink_led(void)
// {
//     /* If the addressable LED is enabled */
//     if (s_led_state) {
//         /* Set the LED pixel using RGB from 0 (0%) to 255 (100%) for each color */
//         led_strip_set_pixel(led_strip, 0, 16, 16, 16);
//         /* Refresh the strip to send data */
//         led_strip_refresh(led_strip);
//     } else {
//         /* Set all LED off to clear all pixels */
//         led_strip_clear(led_strip);
//     }
// }

// static void configure_led(void)
// {
//     ESP_LOGI(TAG, "Example configured to blink addressable LED!");
//     /* LED strip initialization with the GPIO and pixels number*/
//     led_strip_config_t strip_config = {
//         .strip_gpio_num = BLINK_GPIO,
//         .max_leds = 1, // at least one LED on board
//     };
//     led_strip_rmt_config_t rmt_config = {
//         .resolution_hz = 10 * 1000 * 1000, // 10MHz
//     };
//     ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));
//     /* Set all LED off to clear all pixels */
//     led_strip_clear(led_strip);
// }

// #elif CONFIG_BLINK_LED_GPIO

// static void blink_led(void)
// {
//     /* Set the GPIO level according to the state (LOW or HIGH)*/
//     gpio_set_level(BLINK_GPIO, s_led_state);
// }

// static void configure_led(void)
// {
//     ESP_LOGI(TAG, "Example configured to blink GPIO LED!");
//     gpio_reset_pin(BLINK_GPIO);
//     /* Set the GPIO as a push/pull output */
//     gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
// }

// #endif

// void app_main(void)
// {

//     /* Configure the peripheral according to the LED type */
//     configure_led();

//     while (1) {
//         ESP_LOGI(TAG, "Turning the LED %s!", s_led_state == true ? "ON" : "OFF");
//         blink_led();
//         /* Toggle the LED state */
//         s_led_state = !s_led_state;
//         vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
//     }
// }