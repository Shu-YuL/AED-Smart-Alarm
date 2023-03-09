#include "monitoring_device.h"

/* Defining global variables */
static const char *TAG = "Monitoring_device";

char my_MAC[MAC_length]; /* Storage for my mac address */
char my_IP[IP_length]; /* Storage for my ip address */
char http_response[HTTP_RESPONSE_LEN]; // storage for http response message

QueueHandle_t interputQueue;
TaskHandle_t alert_msg_Handle = NULL;

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

/* This function is responsible for sending the MAC address to the Google Sheet */
void myMACto_GS(void *parameters)
{
    char *device_MAC = (char *)parameters;
    char base_GS_url[] = HTTP_GET_URL;
    int retry = 0;

    char BSURL[strlen(base_GS_url)]; /* Array of characters with the size of url */
    sprintf(BSURL, base_GS_url, device_MAC, my_IP);
    ESP_LOGI(TAG, "GET request sent, URL = %s", BSURL);
    esp_http_client_config_t config = {
        .url = BSURL,
        .method = HTTP_METHOD_GET,
        .cert_pem = NULL,
        .event_handler = client_event_get_handler};

    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_http_client_perform(client);

    /* check http response */
    if (strcmp(http_response,"Accepted") == 0)
    {
        ESP_LOGI(TAG,"HTTP GET Request Successed. Response: %s",http_response);
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
        else ESP_LOGI(TAG,"Reached Maximum retry");
    }

    esp_http_client_cleanup(client);

    vTaskDelete(NULL);
}

/* This function retrieves the MAC address from the device and saves it into 'my_MAC' */
void get_MAC(void)
{
    unsigned char mac_base[6] = {0};

    esp_efuse_mac_get_default(mac_base);
    esp_read_mac(mac_base, ESP_MAC_WIFI_STA);

    sprintf(my_MAC, "%02X:%02X:%02X:%02X:%02X:%02X", mac_base[0],mac_base[1],mac_base[2],mac_base[3],mac_base[4],mac_base[5]);
}

static void IRAM_ATTR button_isr_handler(void *args)
{
    int pinNumber = (int)args;
    xQueueSendFromISR(interputQueue, &pinNumber, NULL);
}

void interrupt_task(void *arg)
{
    int pinNumber;
    while (true)
    {
        if (xQueueReceive(interputQueue, &pinNumber, portMAX_DELAY))
        {
            printf("GPIO %d was pressed. The state is %d\n", pinNumber, gpio_get_level(Trigger_PIN));
            printf("My MAC is: %s\n", my_MAC);
            vTaskDelay(200/portTICK_PERIOD_MS);
            xTaskCreate(myMACto_GS, "Send MAC address to Base Station", 8192, &my_MAC, 4, &alert_msg_Handle);
        }
    }
}

void interrupt_init(void)
{
    esp_rom_gpio_pad_select_gpio(Trigger_PIN);
    gpio_set_direction(Trigger_PIN, GPIO_MODE_INPUT);

    /* set pullup */
    // gpio_pulldown_dis(Trigger_PIN);
    // gpio_pullup_en(Trigger_PIN);
    // gpio_set_intr_type(Trigger_PIN, GPIO_INTR_NEGEDGE);

    /* set pulldown */
    gpio_pulldown_en(Trigger_PIN);
    gpio_pullup_dis(Trigger_PIN);
    gpio_set_intr_type(Trigger_PIN, GPIO_INTR_POSEDGE);
    
    interputQueue = xQueueCreate(10, sizeof(int));
    xTaskCreate(interrupt_task, "interrupt_task", 4096, NULL, 1, NULL);

    gpio_install_isr_service(ESP_INR_FLAG_DEFAULT);
    gpio_isr_handler_add(Trigger_PIN, button_isr_handler, (void *)Trigger_PIN);
}