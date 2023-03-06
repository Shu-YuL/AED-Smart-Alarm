/*********************************************************************************************************************
 Group 16: AED Smart Alarm Monitoring Device

 Code Description: The purpose of this code is to send the device's MAC address
                   to the Google Sheet via wi-fi.
**********************************************************************************************************************/
#include "monitoring_device.h"
#include "wifi_include.h"
#include "UWS_include.h"

static const char *TAG = "Main";

#ifdef UWS

/* FreeRTOS event group to signal when we are connected & ready to make a request */
static EventGroupHandle_t wifi_event_group;

/* esp netif object representing the WIFI station */
static esp_netif_t *sta_netif = NULL;


const int wifi_connected = BIT0; //flag indicating whether or not we are connnected to wifi


#ifdef INCLUDE_CA_FILES
/* CA cert, taken from ca.pem
   Client cert, taken from client.crt
   Client key, taken from client.key

   The PEM, CRT and KEY file were provided by the person or organization
   who configured the AP with wpa2 enterprise.

   To embed it in the app binary, the PEM, CRT and KEY file is named
   in the component.mk COMPONENT_EMBED_TXTFILES variable.
*/

extern uint8_t ca_pem_start[] asm("_binary_ca_pem_start");
extern uint8_t ca_pem_end[]   asm("_binary_ca_pem_end");


extern uint8_t client_crt_start[] asm("_binary_client_crt_start");
extern uint8_t client_crt_end[]   asm("_binary_client_crt_end");
extern uint8_t client_key_start[] asm("_binary_client_key_start");
extern uint8_t client_key_end[]   asm("_binary_client_key_end");
#endif;

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

static void check_wifi(void)
{
    esp_netif_ip_info_t ip_info;
    memset(&ip_info, 0, sizeof(esp_netif_ip_info_t));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    wifi_ap_record_t ap_info;
    esp_err_t connection_status = ESP_ERR_WIFI_NOT_CONNECT;
    esp_netif_get_ip_info(sta_netif, &ip_info);

    while ((ip_info.ip.addr) == 0) {
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        connection_status = esp_wifi_sta_get_ap_info(&ap_info);
        esp_netif_get_ip_info(sta_netif, &ip_info);
    }

    if (esp_netif_get_ip_info(sta_netif, &ip_info) == ESP_OK) {
        ESP_LOGI(TAG, "-----------------------");
        ESP_LOGI(TAG, "SSID: %s", ap_info.ssid);
        ESP_LOGI(TAG, "IP Address: "IPSTR, IP2STR(&ip_info.ip));
        //ESP_LOGI(TAG, "MAC Address: %X:%X:%X:%X:%X:%X:" ap_info.bssid[0],ap_info.bssid[1], ap_info.bssid[2], ap_info.bssid[3], ap_info.bssid[4], ap_info.bssid[5]);
        ESP_LOGI(TAG, "-----------------------");
    }

    return;
}

#endif

void app_main(void)
{
    #ifdef HOME // if using wifi off campus
    setup_nvs();
    connect_wifi();
    #endif

    #ifdef UWS // if using UWS wifi
    ESP_ERROR_CHECK( nvs_flash_init() );
    initialize_wifi();
    check_wifi();
    #endif
    
    get_MAC(); /* Function call to get device's MAC address*/
    interrupt_init(); /* Initialize GPIO Interrupt */
}