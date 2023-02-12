#include "base_station.h"
#include "HD44780.h"

static const char *TAG = "Base_station";

char Location_str[LOC_MAX_LEN];
bool Clear_flag = false; // flag for http request

QueueHandle_t interputQueue;

/* HTTP Client event handler. Used here to retrieve response. */
esp_err_t client_event_get_handler(esp_http_client_event_handle_t evt)
{
    switch (evt->event_id)
    {
    case HTTP_EVENT_ON_DATA:
        if (evt->data_len > LOC_MAX_LEN) break;
        sprintf(Location_str,"Location: %.*s", evt->data_len, (char *)evt->data);
        ESP_LOGI(TAG, "%s",Location_str);

        // LCD_Alert(); // Event happened, LCD print location

        break;

    default:
        break;
    }
    return ESP_OK;
}
/* Perform HTTP GET request */
void http_get_task(void *pvParameters)
{
    char default_url[] = HTTP_GET_URL;
    char url[strlen(default_url) + 6];
    /* if Clear_flag set, concatenate "&clear" to the url, and reset Clear_flag */
    if (Clear_flag)
    {
        sprintf(url, "%s&clear", default_url);
        Clear_flag = false;
        ESP_LOGI(TAG,"Read and Clear request set, url: %s",url);
    }
    else // otherwise, url = default url (HTTP_GET_URL)
    {
        sprintf(url, "%s", default_url);
        ESP_LOGI(TAG,"Read request sent, url: %s",url);
    }
    esp_http_client_config_t config_get = {
        .url = url,
        .method = HTTP_METHOD_GET,
        .cert_pem = NULL,
        .event_handler = client_event_get_handler};
        
    esp_http_client_handle_t client = esp_http_client_init(&config_get);
    esp_http_client_perform(client);
    esp_http_client_cleanup(client);

    vTaskDelete(NULL);
}

/* if timer triggered (every 5 sec), perform http get request */
void timer_callback(TimerHandle_t xTimer)
{
    xTaskCreate(http_get_task, "http_get_task", TASK_STACK_SIZE, NULL, 5, NULL);
}

static void IRAM_ATTR gpio_interrupt_handler(void *args)
{
    int pinNumber = (int)args;
    xQueueSendFromISR(interputQueue, &pinNumber, NULL);
}

/* Clear button pressed, set Clear_flag */
void Clear_Flag_Task(void *params)
{
    int pinNumber;
    while (true)
    {
        if (xQueueReceive(interputQueue, &pinNumber, portMAX_DELAY))
        {
            printf("GPIO %d was pressed. The state is %d\n", pinNumber, gpio_get_level(Clear_PIN));
            Clear_flag = true;

            // LCD_Secured(); // Event dismissed, LCD display "Secured"

        }
    }
}

/* Clear_PIN set input, pulldown, pos edge trigger */
void Clear_button(void)
{
    esp_rom_gpio_pad_select_gpio(Clear_PIN);
    gpio_set_direction(Clear_PIN, GPIO_MODE_INPUT);
    gpio_pulldown_en(Clear_PIN);
    gpio_pullup_dis(Clear_PIN);
    gpio_set_intr_type(Clear_PIN, GPIO_INTR_POSEDGE);

    interputQueue = xQueueCreate(10, sizeof(int));
    xTaskCreate(Clear_Flag_Task, "Clear_Flag_Task", 2048, NULL, 1, NULL);

    gpio_install_isr_service(0);
    gpio_isr_handler_add(Clear_PIN, gpio_interrupt_handler, (void *)Clear_PIN);
}

void LCD_Alert(void)
{
    LCD_printf(0, 0, "*** Alarm Alert! ***", NA, NA);
    LCD_printf(2, 0, "%s", NA, Location_str);
}
void LCD_Secured(void)
{
    LCD_printf(2, 0, "------Secured!------", NA, NA);
}