#include "monitoring_device.h"

/* Defining global variables */
static const char *TAG = "Monitoring_device";
// char api_key[] = "5570394"; /* Unique API Key to access the API */
// char whatsapp_num[] = "15879690618"; /* Mobile number that the message will be sent to */
// char aed_alert_msg[] = "***AED Alert***"; /* AED header message */
// char Location [] = "Location: ETLC 2F"; /* Demo of location */
char my_MAC[MAC_length]; /* Storage for my mac address */
char http_response[HTTP_RESPONSE_LEN]; // storage for http response message

QueueHandle_t interputQueue;
TaskHandle_t alert_msg_Handle = NULL;

/* This function interates throught each characted of the received string. The string is then encoded (Transformed into HEX)*/
// char *url_encode(const char *str)
// {
//     static const char *hex = "0123456789abcdef";
//     static char encoded[1024];
//     char *p = encoded;
//     while (*str)
//     {
//         if (*str == '-' || *str == '_' || *str == '.' || *str == '~')
//         {
//             *p++ = *str;
//         }
//         else
//         {
//             *p++ = '%';
//             *p++ = hex[*str >> 4];
//             *p++ = hex[*str & 15];
//         }
//         str++;
//     }
//     *p = '\0';
//     return encoded;
// }

// /* This function is responsible for sending the Whatsapp message */
// void send_whatsapp_message(void *arg)
// {
//     char *alarm_msg = (char *)arg;

//     /* Declaring variable that contains the URL of CallmeBot AP */
//     char callmebot_url[] = "https://api.callmebot.com/whatsapp.php?phone=%s&text=%s&apikey=%s";

//     char URL[strlen(callmebot_url)]; /* Array of characters with the size of callmebot_url */
//     sprintf(URL, callmebot_url, whatsapp_num, url_encode(alarm_msg), api_key);
//     ESP_LOGI(TAG, "URL = %s", URL);
//     esp_http_client_config_t config = {
//         .url = URL,
//         .method = HTTP_METHOD_GET,
//     };
//     esp_http_client_handle_t client = esp_http_client_init(&config);
//     esp_err_t err = esp_http_client_perform(client);

//     if (err == ESP_OK)
//     {
//         int status_code = esp_http_client_get_status_code(client);
//         if (status_code == 200)
//         {
//             ESP_LOGI(TAG, "Message sent Successfully");
//         }
//         else
//         {
//             ESP_LOGI(TAG, "Message sent Failed");
//         }
//     }
//     else
//     {
//         ESP_LOGI(TAG, "Message sent Failed");
//     }
//     esp_http_client_cleanup(client);

//     vTaskDelete(NULL);
// }

/* HTTP Client event handler. Used here to retrieve response. */
esp_err_t client_event_get_handler(esp_http_client_event_handle_t evt)
{
    switch (evt->event_id)
    {
    case HTTP_EVENT_ON_DATA:
        if (evt->data_len > HTTP_RESPONSE_LEN) break;
        sprintf(http_response,"%.*s", evt->data_len, (char *)evt->data);
        ESP_LOGI(TAG, "%s",http_response);

        // LCD_Alert(); // Event happened, LCD print location

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
    /* Declaring variable that contains the URL of CallmeBot AP */
    char base_GS_url[] = HTTP_POST_URL;

    char BSURL[strlen(base_GS_url)]; /* Array of characters with the size of callmebot_url */
    sprintf(BSURL, base_GS_url, device_MAC);
    ESP_LOGI(TAG, "POST request sent, URL = %s", BSURL);
    esp_http_client_config_t config = {
        .url = BSURL,
        .method = HTTP_METHOD_POST,
        .cert_pem = NULL,
        .event_handler = client_event_get_handler};

    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_http_client_perform(client);

    //   ******** check status not working
    if (strcmp(http_response,"Paired") == 1 || strcmp(http_response,"Accepted") == 1)
    {
        ESP_LOGI(TAG,"HTTP POST Request Successed. Response: %s",http_response);
    }
    else
    {
        ESP_LOGI(TAG,"HTTP POST Request Failed");
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

/* Call this function to send messages to whatsapp */
// void send_whatup_message(void)
// {
//     xTaskCreate(send_whatsapp_message, "Send alert notification", 8192, &aed_alert_msg, 12, &alert_msg_Handle);
//     vTaskDelay(200/portTICK_PERIOD_MS);
//     xTaskCreate(send_whatsapp_message, "Send MAC address", 8192, &my_MAC, 8, &alert_msg_Handle);
//     vTaskDelay(200/portTICK_PERIOD_MS);
//     xTaskCreate(send_whatsapp_message, "Send location", 8192, &Location, 4, &alert_msg_Handle);
// }

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