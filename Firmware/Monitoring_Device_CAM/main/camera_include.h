#include <esp_system.h>
#include <nvs_flash.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_camera.h"
#include "esp_http_server.h"
#include "esp_timer.h"
#include "camera_pins.h"
#include "esp_mac.h"
#include "esp_log.h"

#define PART_BOUNDARY "123456789000000000000987654321"
#define CONFIG_XCLK_FREQ 20000000 
/* Function Prototypes */
esp_err_t init_camera(void);
httpd_handle_t setup_server(void);