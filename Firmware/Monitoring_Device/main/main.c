/* Group: 16 AED Smart Alarm - Monitoring Device
 * Module: main
 * Description: This is the main program of our AED Monitoring device. The purpose of this code is to send the
                device's MAC address to the web server via HTTP request.
 * Authors: Pei-Yu Huang, Shu-Yu Lin, Mohammad Kamal
 * References:  - Espressif Official API References
 *                  - https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/index.html
 *                  - https://github.com/espressif/esp-idf/tree/47852846d3e89580ef4da28210b6ffc1cb5eaa9d/examples
 *              - ESP32 Tutorials (admin@esp32tutorials.com)
 *                  - https://esp32tutorials.com/esp32-esp-idf-send-messages-whatsapp/
 *                  - https://www.esp32.com/viewtopic.php?t=21310
 * Revision Information: V.0.0 (First Revision)
 * Date: 10/March/2023
 * Copyright: N/A
 * Functions: - setup_nvs(void)
              - configure_sleep()
              - connect_wifi()
              - initialize_wifi()
              - check_wifi()
              - get_MAC()
              - myMACto_GS()
*/
#include "monitoring_device.h"
#include "wifi_include.h"
#include "UWS_include.h"
/* Defining TAG name for debugging */
static const char *TAG = "Main";

/*------------------------------------------
Function Name: setup_nvs()
Description: Initializes and clears the NVS in the ESP32.
Input: N/A
Output: N/A
Variables Affected: N/A
-------------------------------------------*/
void setup_nvs(void) {
    //Initializes NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
}

#ifdef UWS

/* FreeRTOS event group to signal when we are connected & ready to make a request */
static EventGroupHandle_t wifi_event_group;

/* esp netif object representing the WIFI station */
static esp_netif_t *sta_netif = NULL;


const int wifi_connected = BIT0; //flag indicating whether or not we are connnected to wifi

/*------------------------------------------
Function Name: wifi_event_handler()
Description: Event handler function for using WPA2 WiFi networks like UWS. Calls esp_wifi_connect() 
             to connect to WiFi if we start WiFi or WiFi disconnects.
Input: esp_event_base_t event, int32_t event_id, event_data
Output: N/A
Variables Affected: wifi_connected
-------------------------------------------*/
static void wifi_event_handler(void* arg, esp_event_base_t event, int32_t event_id, void* event_data) {
    
    if (event == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        esp_wifi_connect();
        xEventGroupClearBits(wifi_event_group, wifi_connected);
    } else if (event == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        xEventGroupSetBits(wifi_event_group, wifi_connected);
    }
}
/*------------------------------------------
Function Name: initialize_wifi()
Description: Sets up ESP32 as a WiFi station and configures parameters such as ssid and password, and starts WiFi.
             Used for WPA2 networks like UWS.
Input: N/A
Output: N/A
Variables Affected: N/A
-------------------------------------------*/
static void initialize_wifi(void) {

    #ifdef INCLUDE_CA_FILES
    unsigned int ca_pem_bytes = ca_pem_end - ca_pem_start;

    unsigned int client_crt_bytes = client_crt_end - client_crt_start;
    unsigned int client_key_bytes = client_key_end - client_key_start;
    #endif


    ESP_ERROR_CHECK(esp_netif_init());
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL) );
    ESP_ERROR_CHECK( esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    
    wifi_config_t wifi_config = {.sta = {.ssid = WIFI_SSID,},};

    ESP_LOGI(TAG, "Setting SSID to %s...", wifi_config.sta.ssid);
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
    ESP_ERROR_CHECK( esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)WIFI_EAP_ID, strlen(WIFI_EAP_ID)) );


    ESP_ERROR_CHECK( esp_wifi_sta_wpa2_ent_set_username((uint8_t *)WIFI_EAP_USERNAME, strlen(WIFI_EAP_USERNAME)) );
    ESP_ERROR_CHECK( esp_wifi_sta_wpa2_ent_set_password((uint8_t *)WIFI_EAP_PASSWORD, strlen(WIFI_EAP_PASSWORD)) );


    ESP_ERROR_CHECK( esp_wifi_sta_wpa2_ent_enable() );
    ESP_ERROR_CHECK( esp_wifi_start() );
}
/*------------------------------------------
Function Name: check_wifi()
Description: Checks WiFi connection status and retrieves details such as ssid, ip address and mac address if connected to a WiFi network.
Input: N/A
Output: N/A
Variables Affected: N/A
-------------------------------------------*/
static void check_wifi(void)
{
    esp_netif_ip_info_t ip_info;
    memset(&ip_info, 0, sizeof(esp_netif_ip_info_t));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    wifi_ap_record_t ap_info;
    esp_err_t connection_status = ESP_ERR_WIFI_NOT_CONNECT;
    esp_netif_get_ip_info(sta_netif, &ip_info);

    while ((ip_info.ip.addr) == 0) { // keep checking connection status and waiting until ip address received, indicating a successful connection 
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        connection_status = esp_wifi_sta_get_ap_info(&ap_info);
        esp_netif_get_ip_info(sta_netif, &ip_info);
    }

    if (esp_netif_get_ip_info(sta_netif, &ip_info) == ESP_OK) { // print wifi connection parameters 
        ESP_LOGI(TAG, "-----------------------");
        ESP_LOGI(TAG, "SSID: %s", ap_info.ssid);
        ESP_LOGI(TAG, "IP Address: "IPSTR, IP2STR(&ip_info.ip));
        ESP_LOGI(TAG, "-----------------------");
    }

    return;
}

#endif

TaskHandle_t alert_msg_Handle = NULL;

/*------------------------------------------
Function Name: app_main()
Description: Main program that sets up the Monitoring Device. 
Input: N/A
Output: N/A
Variables Affected: N/A
-------------------------------------------*/
void app_main(void)
{
    configure_sleep();

    setup_nvs();

    get_MAC(); /* Function call to get device's MAC address*/

    #ifdef HOME // if using wifi off campus
    connect_wifi();
    #endif

    #ifdef UWS // if using UWS wifi
    initialize_wifi();
    check_wifi();
    #endif
    
    xTaskCreate(myMACto_GS, "Send MAC address to Base Station", 8192, &my_MAC, 4, &alert_msg_Handle); /* Call subroutine to send MAC address to base station */
}