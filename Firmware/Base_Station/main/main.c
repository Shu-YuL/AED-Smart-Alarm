#include "wifi_include.h"
#include "HD44780.h"
#include "base_station.h"

static const char *TAG = "Main";

void app_main(void)
{
    // LCD_init(LCD_ADDR, SDA_PIN, SCL_PIN, LCD_COLS, LCD_ROWS); /* Initializing LCD parameters */
    // LCD_home(); /* This function is called before we want to output anything to the screen */
    // LCD_clearScreen(); /* This function is called to clear the current screen */
    // LCD_Secured();

    setup_nvs();
    connect_wifi();

    /* 5s timer interrupt task */
    TimerHandle_t timer = xTimerCreate("timer", pdMS_TO_TICKS(TIMER_PERIOD_MS), pdTRUE, 0, timer_callback);
    xTimerStart(timer, 0);
    
    /* Clear button GPIO interrupt setup */
    Clear_button();
}