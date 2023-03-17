#include "Buzzer_include.h"

static const char *TAG = "Buzzer";

QueueHandle_t buzzer_but_interputQueue;

void buzzer_pwm_init(void)
{
    // Prepare and then apply the LEDC PWM timer configuration
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .timer_num        = LEDC_TIMER,
        .duty_resolution  = LEDC_DUTY_RES,
        .freq_hz          = LEDC_FREQUENCY,  // Set output frequency at 5 kHz
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));
    ESP_LOGI(TAG, "PWM timer config completed");

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = LEDC_OUTPUT_IO,
        .duty           = 0, // Set duty to 0%
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));

    ESP_LOGI(TAG, "PWM channel config completed");
}

static void IRAM_ATTR buzz_but_interrupt_handler(void *args)
{
    int pinNumber = (int)args;
    xQueueSendFromISR(buzzer_but_interputQueue, &pinNumber, NULL);
}

void Buzzer_button_Task(void *params)
{
    int pinNumber;
    while (true)
    {
        if (xQueueReceive(buzzer_but_interputQueue, &pinNumber, portMAX_DELAY))
        {
            printf("GPIO %d was pressed. The state is %d\n", pinNumber, gpio_get_level(Buzzer_but_PIN));
            /* turn off the buzzer output */
            ledc_stop(LEDC_MODE, LEDC_CHANNEL,0);
            ledc_timer_pause(LEDC_MODE, LEDC_CHANNEL);
            
            ESP_LOGI(TAG, "LEDC stopped, buzzer output OFF");
        }
    }
}

void Buzzer_button_config(void)
{
    esp_rom_gpio_pad_select_gpio(Buzzer_but_PIN);
    gpio_set_direction(Buzzer_but_PIN, GPIO_MODE_INPUT);
    gpio_pulldown_en(Buzzer_but_PIN);
    gpio_pullup_dis(Buzzer_but_PIN);
    gpio_set_intr_type(Buzzer_but_PIN, GPIO_INTR_POSEDGE);

    buzzer_but_interputQueue = xQueueCreate(10, sizeof(int));
    xTaskCreate(Buzzer_button_Task, "Buzzer_button_Task", 2048, NULL, 1, NULL);

    gpio_isr_handler_add(Buzzer_but_PIN, buzz_but_interrupt_handler, (void *)Buzzer_but_PIN);
    ESP_LOGI(TAG, "Buzzer button config completed");
}