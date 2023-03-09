/*********************************************************************************************************************
 Group 16: AED Smart Alarm Monitoring Device

 Code Description: The purpose of this code is to send the device's MAC address
                   to the Google Sheet via wi-fi.
**********************************************************************************************************************/
#include "monitoring_device.h"
#include "wifi_include.h"
#include "camera_include.h"

static const char *TAG = "Main";

void setup_nvs(void) {
    //Initializes NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
}

void app_main(void)
{
    esp_err_t err;

    setup_nvs();
    connect_wifi();
    
    get_MAC(); /* Function call to get device's MAC address*/
    interrupt_init(); /* Initialize GPIO Interrupt */

    err = init_camera();
    if (err != ESP_OK)
    {
        printf("err: %s\n", esp_err_to_name(err));
        return;
    }
    setup_server();
    ESP_LOGI(TAG, "ESP32 CAM Web Server is up and running\n");
}