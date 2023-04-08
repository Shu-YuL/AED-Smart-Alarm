#include "base_station.h"
#include "HD44780.h"
#include "Buzzer_include.h"
#include "led.h"

static const char *TAG = "Base_station";

char Location_str[LOC_MAX_LEN]; //location of triggered alarm 
char temp_str[LOC_MAX_LEN]; // buffer for storing the last location value
bool Clear_flag = false; // flag for http request

int buzzer_pause_count = 0;

QueueHandle_t interputQueue;


/*------------------------------------------
Function Name: client_event_get_handler()
Description: HTTP Client event handler. Used here to retrieve response from
             Google Apps and enable base station alarm if an alarm is triggered.
Input: evt of type esp_http_client_event_handle_t
Output: N/A
Variables Affected: Location_str
-------------------------------------------*/
esp_err_t client_event_get_handler(esp_http_client_event_handle_t evt)
{
    switch (evt->event_id)
    {
    case HTTP_EVENT_ON_DATA:
        /* if received data len exceed acceptable length, discard the data */
        if (evt->data_len > LOC_MAX_LEN) break;
        /* create buffer to store new location value */
        char temp_buffer[LOC_MAX_LEN] = {'\0'};
        sprintf(temp_buffer,"%.*s", evt->data_len, (char *)evt->data);
        /* if the new location string is different from the previous one */
        if (strcmp(temp_str,temp_buffer) != 0)
        {
            /* update temp_str to store the new location value */
            sprintf(temp_str, "%s", temp_buffer);

            if (buzzer_pause_count == 0) {
            /* new location, so turn on the buzzer output */
            ledc_timer_resume(LEDC_MODE, LEDC_CHANNEL);
            ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
            ESP_LOGI(TAG, "LEDC Resumed, buzzer output ON");
            }

            /* set alarm LED to flashing red */
            turn_off_green_LED();
            flash_red_LED();
        
        }

        /* update Location_str to the new location value */
        sprintf(Location_str,"%.*s", evt->data_len, (char *)evt->data);
        ESP_LOGI(TAG, "Location: %s",Location_str);

        LCD_Alert(); // Event happened, LCD print location


        break;

    default:
        break;
    }
    return ESP_OK;
}


/*------------------------------------------
Function Name: http_get_task()
Description: Sends an HTTP GET request to Google Apps. If Clear_flag is set, sends a HTTP request to clear the Spreadsheet.
            Disables buzzzer and LED if no alarm triggered (i.e. Location_str is empty).
Input: N/A
Output: N/A
Variables Affected: url, buzzer_pause_count, Location_str
-------------------------------------------*/
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
 
    if (buzzer_pause_count)
    {
        buzzer_pause_count--;
    }

    /* if Location_str is a empty string (no event occurred) */
    if (strcmp(Location_str,"\0") == 0)
    {
        LCD_Secured(); // LCD secured mode

        /* turn off the buzzer output */
        ledc_stop(LEDC_MODE, LEDC_CHANNEL,0);
        ledc_timer_pause(LEDC_MODE, LEDC_CHANNEL);
        ESP_LOGI(TAG, "LEDC stopped, buzzer output OFF");

        /* set alarm LED to green*/
        turn_off_red_LED();
        turn_on_green_LED();
    }

    memset(Location_str,0,sizeof(Location_str)); // Clear Location_str

    vTaskDelete(NULL);
}


/*------------------------------------------
Function Name: timer_callback()
Description: If timer triggered (every 5 sec), perform HTTP GET request.
Input: xTimer of type TimerHandle_t
Output: N/A
Variables Affected: N/A
-------------------------------------------*/
void timer_callback(TimerHandle_t xTimer)
{
    xTaskCreate(http_get_task, "http_get_task", TASK_STACK_SIZE, NULL, 5, NULL);
}


/*------------------------------------------
Function Name: gpio_interrupt_handler()
Description: Interrupt routine handler when a button is pressed on the base station. 
Input: N/A
Output: N/A
Variables Affected: N/A
-------------------------------------------*/
static void IRAM_ATTR gpio_interrupt_handler(void *args)
{
    int pinNumber = (int)args;
    xQueueSendFromISR(interputQueue, &pinNumber, NULL);
}


/*------------------------------------------
Function Name: Clear_Flag_Task()
Description: Sets Clear_Flag to indicate that user has requested to clear an active alarm and disables the buzzer and LED 
             when the clear button is pressed.
Input: params
Output: N/A
Variables Affected: Clear_flag, buzzer_pause_count
-------------------------------------------*/
void Clear_Flag_Task(void *params)
{
    int pinNumber;
    
    while (true)
    {
        if (xQueueReceive(interputQueue, &pinNumber, portMAX_DELAY))
        {
            printf("GPIO %d was pressed. The state is %d\n", pinNumber, gpio_get_level(Clear_PIN));
            Clear_flag = true;
            buzzer_pause_count = 2;
            /* turn off the buzzer output */
            ledc_stop(LEDC_MODE, LEDC_CHANNEL,0);
            ledc_timer_pause(LEDC_MODE, LEDC_CHANNEL);
            ESP_LOGI(TAG, "LEDC stopped, buzzer output OFF");
        }
    }
}


/*------------------------------------------
Function Name: Clear_button()
Description: Sets the Clear_button GPIO pin as a pull-down enabled, positive-edge triggered input. 
             Attaches gpio_interrupt_handler() as the interrupt routine when the clear button is pressed.
Input: N/A
Output: N/A
Variables Affected: N/A
-------------------------------------------*/
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


/*------------------------------------------
Function Name: LCD_Alert()
Description: Prints an alert message with the location on the LCD screen. 
             Call this function when a triggered alarm is found in the Spreadsheet.
Input: N/A
Output: N/A
Variables Affected: N/A
-------------------------------------------*/
void LCD_Alert(void)
{
    LCD_clearScreen();
    LCD_printf(0, 0, "***    Alert!    ***", NA, NA);
    LCD_printf(1, 0, "Location:", NA, NA);
    LCD_printf(2, 0, "", NA, Location_str);
}


/*------------------------------------------
Function Name: LCD_Secured()
Description: Clears the LCD screen to indicate no active alarm. Call this function when there is no active alarm in the Spreadsheet.
Input: N/A
Output: N/A
Variables Affected: N/A
-------------------------------------------*/
void LCD_Secured(void)
{   
    LCD_clearScreen();
    LCD_printf(1, 0, "------Secured!------", NA, NA);
}