#include "stm32l476xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include <string.h>
#include "common/utils.h"
#include "drivers/io.h"

/* TEST OPTIONS
 * Comment-in any features or peripherals
 * that you want to test. Any combination
 * of features is intended to work together.
 */
#define LED_TEST
#define UART_TEST
// #define LCD_TEST
// #define SPI_OLED_TEST

/*****************************************************/
/* LED_TEST */
/*****************************************************/
#ifdef LED_TEST
#include "drivers/led.h"
#endif // LED_TEST

/*****************************************************/
/* UART_TEST */
/*****************************************************/
#ifdef UART_TEST
#include "drivers/uart.h"
#endif // UART_TEST

/*****************************************************/
/* LCD_TEST */
/*****************************************************/
#ifdef LCD_TEST
#include "drivers/lcd.h"
#endif // LCD_TEST

/*****************************************************/
/* SPI_OLED_TEST */
/*****************************************************/
#ifdef SPI_OLED_TEST
#include "drivers/spi1.h"
#include "drivers/oled.h"
#include "app/oled_fonts/FreeMono9pt7b.h"
#endif // SPI_OLED_TEST

int main(void)
{
    // clang-format off
    #ifdef LED_TEST
    // *******************************************
    led_init();
    led_create_tasks();
    #endif // LED_TEST

    #ifdef UART_TEST
    // *******************************************
    uart_init();
    uart_create_tasks();
    #endif // UART_TEST

    #ifdef LCD_TEST
    // *******************************************
    lcd_init();
    lcd_create_tasks();
    
    // Initial LCD write
    char *lcd_init_string = "Ryan Beck";
    lcd_transmit_string(lcd_init_string, strlen(lcd_init_string));
    lcd_set_cursor(1,0);
    #endif // LCD_TEST

    #ifdef SPI_OLED_TEST
    // *******************************************
    oled_init();
    oled_create_tasks();

    // Initial OLED write
    oled_draw_string(0, 10, "Hello World!", &FreeMono9pt7b, true);
    oled_flush();
    delay_ms(2000);
    oled_clear();
    oled_flush();
    #endif // SPI_OLED_TEST

    // clang-format on

    /* Enable fault handlers */
    SCB->SHCSR |= SCB_SHCSR_USGFAULTENA_Msk | SCB_SHCSR_BUSFAULTENA_Msk | SCB_SHCSR_MEMFAULTENA_Msk;

    vTaskStartScheduler();
    for (;;) { }
}
