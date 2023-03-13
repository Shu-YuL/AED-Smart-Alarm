/* Group: 16 AED Smart Alarm - Monitoring Device
 * Module: main
 * Description: This is the main program of our AED Monitoring Device with Camera. The purpose of this code is to send the
                device's MAC and IP address to the base station's web server via wi-fi.
 * Collaborators: Pei-Yu Huang, Shu-Yu Lin, Mohammad Kamal
 * Author: - ESP32 Tutorials
 * Contact: - admin@esp32tutorials.com
 * Sources/URLs: - https://esp32tutorials.com/esp32-esp-idf-send-messages-whatsapp/
                 - https://esp32tutorials.com/esp32-cam-esp-idf-live-streaming-web-server/#more-2787
 * Revision Information: V.0.0 (First Revision)
 * Date: 12/March/2023
 * Copyright: N/A
 * Functions: - setup_nvs(void)
              - connect_wifi()
              - initialize_wifi()
              - check_wifi()
              - get_MAC()
              - setup_server()
              - init_camera()
*/

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

    err = init_camera(); /* Function call to setup camera */
    if (err != ESP_OK)
    {
        printf("err: %s\n", esp_err_to_name(err));
        return;
    }
    setup_server(); /* Function call to setup web server */
    ESP_LOGI(TAG, "ESP32 CAM Web Server is up and running\n");
}