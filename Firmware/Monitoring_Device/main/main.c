/*********************************************************************************************************************
 Group 16: AED Smart Alarm Monitoring Device

 Code Description: The purpose of this code is to send the device's MAC address
                   to the base station via wi-fi.
 
 References: The code is a combination of multiple open sources code available
             publicly on GitHub, ESP32 ESP-IDF Tutorials, and ESPRESSIF forum.
             The list of repositories and websites are listed below:
             - Retrieving MAC address from ESP32: https://www.esp32.com/viewtopic.php?t=21310
             - Create Tasks and Interrupts: https://esp32tutorials.com/esp32-esp-idf-freertos-tutorial-create-tasks/
             - Send Whatsapp Messages: https://esp32tutorials.com/esp32-esp-idf-send-messages-whatsapp/
             We have made changes to the code to meet the need of this project.
             We do not hold ownership of the code and functions created, and this project
             is solely for personal use and not for distribution.
**********************************************************************************************************************/
#include "monitoring_device.h"
#include "wifi_include.h"

static const char *TAG = "Main";

void app_main(void)
{
    /* Function Initializations */
    setup_nvs(); /* Initializing non-volatile storage */
    connect_wifi(); /* Function call to connect to wifi */
    get_MAC(); /* Function call to get device's MAC address*/
    interrupt_init(); /* Initialize GPIO Interrupt */
}