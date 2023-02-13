/*********************************************************************************************************************
 Group 16: AED Smart Alarm Monitoring Device

 Code Description: The purpose of this code is to send the device's MAC address
                   to the Google Sheet via wi-fi.
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