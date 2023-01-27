#include "wifi_include.h"

static const char *TAG = "WIFI";

/***** WIFI Statics *****/
void setup_led(void) {
    ESP_LOGI(TAG, "LED on GPIO %d set to indicate successful Wi-Fi connection", LED_GPIO);
    gpio_reset_pin(LED_GPIO);
    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);
}

void setup_nvs(void) {
    //Initializes NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
}

#if MD
void setup_wifi(void) {
    //to setup ESP32 as a station (STA) to a simple non-WPA2 access point (AP) like home Wi-Fi 

    wifi_init_config_t init_config = WIFI_INIT_CONFIG_DEFAULT(); 
    //ESP_ERROR_CHECK() checks the esp_err_t return value of API functions, and prints error message if function doesn't return ESP_OK and aborts
    //to avoid aborting, use ESP_ERROR_CHECK_WITHOUT_ABORT()
    //see Espressif's Error Handling Doc: "Many ESP-IDF examples use ESP_ERROR_CHECK to handle errors from various APIs. This is not the best practice for applications, and is done to make example code more concise." May be simpler to just check return values manually
    ESP_ERROR_CHECK(esp_wifi_init(&init_config)); //must be called first
    
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA)); //set ESP32 to be a station (STA); to set as AP, use WIFI_MODE_AP

    //wifi_config_t wifi_configuration;
    //wifi_configuration.sta = {.ssid = WIFI_SSID , .password = WIFI_PASSWORD, .threshold.authmode = WIFI_AUTHORIZATION_MODE_THRESHOLD, .sae_pwe_h2e = WPA3_SAE_PWE_BOTH,};


    wifi_config_t wifi_configuration = {
        .sta = {.ssid = WIFI_SSID, .password = WIFI_PASSWORD,
            /* Setting a password implies station will connect to all security modes including WEP/WPA.
             * However these modes are deprecated and not advisable to be used. Incase your Access point
             * doesn't support WPA2, these mode can be enabled by commenting below line */
	     .threshold.authmode = WIFI_AUTH_OPEN, .sae_pwe_h2e = WPA3_SAE_PWE_BOTH,
         },
    };

    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_configuration));

    ESP_ERROR_CHECK(esp_wifi_start());
    
    return;
}

esp_err_t connect_wifi(void) {
    //attempts to connect ESP32 to a WiFi Access Point specified by setup_wifi()
    
    esp_err_t wifi_connection_status = !ESP_OK;
    uint8_t tries = 0;

    while (wifi_connection_status != ESP_OK && tries < WIFI_MAX_TRIES ) {  //attempt to connect to AP until connection established, or max attempts reached
        wifi_connection_status = esp_wifi_connect();
        //vTaskDelay(2000 / portTICK_PERIOD_MS); //wait 2s before retrying to connect to AP
        tries++;
    }


    if (wifi_connection_status == ESP_OK) {
        ESP_LOGI(TAG, "WiFi Successfully Connected!");
        wifi_ap_record_t ap_info;
        esp_wifi_sta_get_ap_info(&ap_info);
        ESP_LOGI(TAG, "__________________________________________");
        ESP_LOGI(TAG, "AP SSID: %s, AP MAC Address: %s, rssi: %d ", ap_info.ssid, ap_info.bssid, ap_info.rssi);
        gpio_set_level(LED_GPIO, 1);
    }
    else {ESP_LOGI(TAG, "WiFi Failed to Connected");
    gpio_set_level(LED_GPIO, 0);
    }
    
    return wifi_connection_status;
}

#endif