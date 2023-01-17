/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "driver/i2c.h"
#include "esp_err.h"
#include "esp_log.h"
#include "lvgl.h"
#include "esp_lvgl_port.h"
#include "esp_lcd_panel_vendor.h"
#include "oled_init.h"
#include "oled_functions.h"

#include "./examples/scroll/lv_example_scroll_1.c"

/* Global Function Declaration */
lv_disp_t *assign_disp;

void app_main(void)
{
    /* Initialization. I2C Interfaced OLED setup. Returns the address of the assigned display */
    assign_disp = oled_init();

    lv_obj_clean(lv_scr_act()); // clear screen
    oled_printf(assign_disp, 0, 0, "2nd Floor");
    oled_printf(assign_disp, 0, 20, "ETLC");
    oled_printf(assign_disp, 0, 40, "Bat Medium");
}
