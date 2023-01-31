#include "wifi_include.h"
#include "server_include.h"

#include "ssd1306.h"
#include "font8x8_basic.h"

static const char *TAG = "Main";

/***** Wifi Statics *****/
/* FreeRTOS event group to signal when wifi is connected*/
static EventGroupHandle_t s_wifi_event_group;

static int s_retry_num = 0;

static void event_handler(void *arg, esp_event_base_t event_base,
                          int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY)
        {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        }
        else
        {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG, "connect to the AP fail");
        ssd1306_clear_screen(&dev, false);
        oled_printf(&dev, 1, "Failed to");
        oled_printf(&dev, 2, "connect to AP");
        oled_printf(&dev, 4, "Check hardware");
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

void connect_wifi(void)
{
    s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_got_ip));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .password = EXAMPLE_ESP_WIFI_PASS,
            /* Setting a password implies station will connect to all security modes including WEP/WPA.
             * However these modes are deprecated and not advisable to be used. Incase your Access point
             * doesn't support WPA2, these mode can be enabled by commenting below line */
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_sta finished.");

    /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
     * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
                                           WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                           pdFALSE,
                                           pdFALSE,
                                           portMAX_DELAY);

    /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
     * happened. */
    if (bits & WIFI_CONNECTED_BIT)
    {
        ESP_LOGI(TAG, "connected to ap SSID:%s password:%s",
                 EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
    }
    else if (bits & WIFI_FAIL_BIT)
    {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s",
                 EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
        ssd1306_clear_screen(&dev, false);
        oled_printf(&dev, 1, "Failed to");
        oled_printf(&dev, 2, "connect to WIFI");
        oled_printf(&dev, 4, "Check hardware");
    }
    else
    {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
        ssd1306_clear_screen(&dev, false);
        oled_printf(&dev, 1, "UNEXPECTED EVENT");
        oled_printf(&dev, 3, "Check hardware");
    }
    vEventGroupDelete(s_wifi_event_group);
}

SSD1306_t dev; // Global variable for oled device address

void app_main(void)
{
    /* oled init */
    int center, top, bottom;
    char lineChar[20];

    ESP_LOGI(TAG, "INTERFACE is i2c");
    ESP_LOGI(TAG, "CONFIG_SDA_GPIO=%d",CONFIG_SDA_GPIO);
    ESP_LOGI(TAG, "CONFIG_SCL_GPIO=%d",CONFIG_SCL_GPIO);
    ESP_LOGI(TAG, "CONFIG_RESET_GPIO=%d",CONFIG_RESET_GPIO);
    i2c_master_init(&dev, CONFIG_SDA_GPIO, CONFIG_SCL_GPIO, CONFIG_RESET_GPIO);

#if CONFIG_FLIP
    dev._flip = true;
    ESP_LOGW(tag, "Flip upside down");
#endif

    ssd1306_init(&dev, 128, 64);

    ssd1306_clear_screen(&dev, false);

    /* Wifi initialization */
    oled_printf(&dev, 0, "1 Starting...");
    vTaskDelay(1000/ portTICK_PERIOD_MS); // delay 1s

    setup_nvs();
    oled_printf(&dev, 1, "2 NVS OK");
    vTaskDelay(1000/ portTICK_PERIOD_MS); // delay 1s

    /* Server */
    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
    connect_wifi();

    oled_printf(&dev, 2, "3 WIFI OK");
    vTaskDelay(1000/ portTICK_PERIOD_MS); // delay 1s

    setup_server();

    oled_printf(&dev, 3, "4 SERVER MAIN");
    vTaskDelay(1000/ portTICK_PERIOD_MS); // delay 1s
}