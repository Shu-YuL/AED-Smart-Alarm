#include "monitoring_device.h"

/* Defining global variables */
static const char *TAG = "Monitoring_device";

char my_MAC[MAC_length]; /* Storage for my mac address */
char http_response[HTTP_RESPONSE_LEN]; /* storage for http response message */

QueueHandle_t interputQueue;

/* HTTP Client event handler. Used here to retrieve response. */
esp_err_t client_event_get_handler(esp_http_client_event_handle_t evt)
{
    switch (evt->event_id)
    {
    case HTTP_EVENT_ON_DATA:
        if (evt->data_len > HTTP_RESPONSE_LEN) break;
        sprintf(http_response,"%.*s", evt->data_len, (char *)evt->data);
        ESP_LOGI(TAG, "resp = %s",http_response);
        ESP_LOGI(TAG, "len = %d", evt->data_len);

        break;

    default:
        break;
    }
    return ESP_OK;
}

/* -----------------------------------------------------------------------------------------
 * Subroutine Name: myMACto_GS
 * Description: This function is responsible for sending the device's MAC address to the
                base station. The MAC address will be registered in Google Sheets.
 * Input: Pointer to'parameters' -> address contains the device's MAC
 * Output: esp_http_client_handle_t, NULL if any errors
 * Registers Affected: N/A
 ----------------------------------------------------------------------------------------- */
void myMACto_GS(void *parameters)
{
    char *device_MAC = (char *)parameters;
    char base_GS_url[] = HTTP_GET_URL;
    int retry = 0;

    char BSURL[strlen(base_GS_url)]; /* Array of characters with the size of url */
    sprintf(BSURL, base_GS_url, device_MAC);
    ESP_LOGI(TAG, "GET request sent, URL = %s", BSURL);

    /* Specifying http method */
    esp_http_client_config_t config = {
        .url = BSURL,
        .method = HTTP_METHOD_GET,
        .cert_pem = NULL,
        .event_handler = client_event_get_handler};

    /* The following function is used as input to ther functions in the interface.
       The function returns esp_http_client_handle_t, NULL if any errors */
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_http_client_perform(client);

    /* Check HTTP response */
    if (strcmp(http_response,"Accepted") == 0)
    {
        ESP_LOGI(TAG,"HTTP GET Request Successed. Response: %s",http_response);

        enter_deep_sleep(); // go to sleep if http request is accepted
    }
    else if (strcmp(http_response,"Paired") == 0)
    {
        ESP_LOGI(TAG,"HTTP GET Request Successed. Response: %s",http_response);
    }
    else
    {
        ESP_LOGI(TAG,"HTTP GET Request Failed. Response: %s",http_response);
        if (retry <= 5)
        {
            /* retry http request */
            esp_http_client_perform(client);
            retry++;
        }
        else ESP_LOGI(TAG,"Reached Maximum retry"); /* Error message */
    }

    esp_http_client_cleanup(client); /* Operation is complete */

    vTaskDelete(NULL); /* Delete task when done to free memory */
}

/* -----------------------------------------------------------------------------------------
 * Subroutine Name: get_MAC
 * Description: This function retrieves the MAC address from the device and saves it into
                'my_MAC' 
 * Input: void
 * Output: void
 * Registers Affected: N/A
 ----------------------------------------------------------------------------------------- */
void get_MAC(void)
{
    unsigned char mac_base[6] = {0};

    esp_efuse_mac_get_default(mac_base);
    esp_read_mac(mac_base, ESP_MAC_WIFI_STA);

    sprintf(my_MAC, "%02X:%02X:%02X:%02X:%02X:%02X", mac_base[0],mac_base[1],mac_base[2],mac_base[3],mac_base[4],mac_base[5]);
}

/* call configure_sleep() near the beginning of app_main() */
void configure_sleep(void) {
 
    printf("Configuring EXT0 wakeup on GPIO pin %d\n", WAKEUP_PIN);

    ESP_ERROR_CHECK(esp_sleep_enable_ext0_wakeup(WAKEUP_PIN, 1)); //WAKEUP_PIN used as interrupt to wake up from deep sleep

    // Configure pullup/downs via RTCIO to tie wakeup pins to inactive level during deep sleep.
    // EXT0 resides in the same power domain (RTC_PERIPH) as the RTC IO pullup/downs.
    ESP_ERROR_CHECK(rtc_gpio_pullup_dis(WAKEUP_PIN));
    ESP_ERROR_CHECK(rtc_gpio_pulldown_en(WAKEUP_PIN));

    // Isolate GPIO12 pin from external circuits. This is needed for modules
    // which have an external pull-up resistor on GPIO12 (such as ESP32-WROVER)
    // to minimize current consumption.
    rtc_gpio_isolate(GPIO_NUM_12);
}

/* Call enter_deep_sleep() when you want to enter deep sleep */
void enter_deep_sleep(void) {
    esp_wifi_stop(); //shut down WiFi controller  
    esp_deep_sleep_start(); 
}