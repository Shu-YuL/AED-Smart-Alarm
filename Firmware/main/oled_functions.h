#include <stdio.h>
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

/* Includes from "managed components" */
#include "./widgets/lv_label.c"
#include "./widgets/lv_label.h"

/* Function Declaration */
void oled_printf(lv_disp_t *assign_disp, uint8_t x_position, uint8_t y_position, const char * fmt, ...);

/* This function will print any character to the OLED display. The first argument is always assign_disp
The second argument is the x position, the third argument is the y position, and the last argument is whatever
you wanna write to the OLED display. Please include " " for the last argument */
void oled_printf(lv_disp_t *assign_disp, uint8_t x_position, uint8_t y_position, const char * fmt, ...)
{
    lv_obj_t *scr = lv_disp_get_scr_act(assign_disp);
    lv_obj_t *act_pointer = lv_label_create(scr);

    LV_ASSERT_OBJ(act_pointer, MY_CLASS);
    LV_ASSERT_NULL(fmt);

    lv_obj_invalidate(act_pointer);
    lv_label_t * label = (lv_label_t *)act_pointer;

    //If text is NULL then refresh
    if(fmt == NULL) {
        lv_label_refr_text(act_pointer);
        return;
    }

    if(label->text != NULL && label->static_txt == 0) {
        lv_mem_free(label->text);
        label->text = NULL;
    }

    va_list args;
    va_start(args, fmt);
    label->text = _lv_txt_set_text_vfmt(fmt, args);
    va_end(args);
    label->static_txt = 0; //Now the text is dynamically allocated

    lv_label_refr_text(act_pointer);
    lv_obj_set_width(act_pointer, assign_disp->driver->hor_res);
    lv_obj_align(act_pointer, LV_ALIGN_TOP_MID, x_position, y_position);
}