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

void call(void *arg);

/* Global Function Declaration */
lv_disp_t *assign_disp;

/* The LVGL port component calls esp_lcd_panel_draw_bitmap API for send data to the screen. There must be called
lvgl_port_flush_ready(disp) after each transaction to display. The best way is to use on_color_trans_done
callback from esp_lcd IO config structure. In IDF 5.1 and higher, it is solved inside LVGL port component. */

static bool notify_lvgl_flush_ready(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t *edata, void *user_ctx)
{
    lv_disp_t * disp = (lv_disp_t *)user_ctx;
    lvgl_port_flush_ready(disp);
    return false;
}

void app_main(void)
{
    /* Initialization. I2C Interfaced OLED setup. Returns the address of the assigned display */
    assign_disp = oled_init();

    lv_obj_clean(lv_scr_act()); // clear screen
    oled_printf(assign_disp, 0, 0, "2nd Floor");
    //vTaskDelay(3000/portTICK_PERIOD_MS);
    oled_printf(assign_disp, 0, 20, "ETLC");
    //vTaskDelay(3000/portTICK_PERIOD_MS);
    oled_printf(assign_disp, 0, 40, "Bat Medium");
}
