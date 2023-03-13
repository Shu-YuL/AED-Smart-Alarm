#include "monitoring_device.h"

/* Defining TAG name for debugging */
static const char *TAG = "Monitoring_device";
/* Defining static global variables */
char my_MAC[MAC_length]; /* Storage for my mac address */
char my_IP[IP_length]; /* Storage for my ip address */
char http_response[HTTP_RESPONSE_LEN]; // storage for http response message

QueueHandle_t interputQueue;
TaskHandle_t alert_msg_Handle = NULL;

/* -----------------------------------------------------------------------------------------
 * Subroutine Name: client_event_get_handler
 * Description: HTTP Client event handler. Used here to retrieve HTTP request response.
 * Input: evt
 * Output: ESP_OK return if successfully retrives HTTP response
 * Registers Affected: N/A
 ----------------------------------------------------------------------------------------- */
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
                web server (Google Sheet). The MAC address will be registered in Google Sheets.
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
    sprintf(BSURL, base_GS_url, device_MAC, my_IP);
    ESP_LOGI(TAG, "GET request sent, URL = %s", BSURL);

    /* Configuring http method */
    esp_http_client_config_t config = {
        .url = BSURL,
        .method = HTTP_METHOD_GET,
        .cert_pem = NULL,
        .event_handler = client_event_get_handler};

    /* The following functions initialize HTTP client and then perform an HTTP request.
       The function returns NULL if HTTP request performed with no errors returned */
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_http_client_perform(client);

    /* Check HTTP response */
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
        else ESP_LOGI(TAG,"Reached Maximum retry"); /* Error message */
    }

    esp_http_client_cleanup(client); /* Operation is complete */

    vTaskDelete(NULL); /* Delete task when done */
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

/* -----------------------------------------------------------------------------------------
 * Subroutine Name: button_isr_handler
 * Description: This function is the interrupt service routine (ISR). Routine performed 
 *              whenever the assigned interrupt event occurs, in this case it's the button 
 *              status.
 * Input: pointer 'args' point to the address containing the GPIO pin number
 * Output: void
 * Registers Affected: N/A
 ----------------------------------------------------------------------------------------- */
static void IRAM_ATTR button_isr_handler(void *args)
{
    int pinNumber = (int)args;

    /* xQueueSendFromISR will send an item to a queue. It takes three parameters. The first
       parameter is the queue handle on which the item is to be posted. The second parameter
       is the GPIO interrupt number. Finally, the third parameter is the priority number which
       is NULL in our case. */
    xQueueSendFromISR(interputQueue, &pinNumber, NULL);
}

/* -----------------------------------------------------------------------------------------
 * Subroutine Name: interrupt_task
 * Description: When the interrupt occurs, this function is called to send the device's MAC
                address to the Base Station. 
 * Input: pointer 'args' point to the address of the buffer into which the receive item is
          copied
 * Output: void
 * Registers Affected: N/A
 ----------------------------------------------------------------------------------------- */
void interrupt_task(void *arg)
{
    int pinNumber;
    while (true)
    {
        /* xQueueReceive checks if an item is received from the queue. 
           It takes three parameters. The first parameter is the queue handle on which the
           item is to be received. The second parameter is the pointer to the buffer into which
           the receive item is copied. Finally, the third parameter specifies the max amount of
           time the task should block waiting for an item to receive */
        if (xQueueReceive(interputQueue, &pinNumber, portMAX_DELAY))
        {
            printf("GPIO %d was pressed. The state is %d\n", pinNumber, gpio_get_level(Trigger_PIN));
            printf("My MAC is: %s\n", my_MAC);
            vTaskDelay(200/portTICK_PERIOD_MS);
            /* Perform function 'myMACto_GS' */
            xTaskCreate(myMACto_GS, "Send MAC address to Base Station", 8192, &my_MAC, 4, &alert_msg_Handle);
        }
    }
}

/* -----------------------------------------------------------------------------------------
 * Subroutine Name: interrupt_init
 * Description: This function initialize external interrupts. We setup parameters such as
                triggering mode, interrupt type, and pull-up/pull-down. 
 * Input: void
 * Output: void
 * Registers Affected: - General Purpose Input Output register
                       - Interrupt Control Register
 ----------------------------------------------------------------------------------------- */
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