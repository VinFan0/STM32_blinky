

#include <stdint.h>
#include <string.h>
#include "stm32l476xx.h"
#include "common/utils.h"
#include "drivers/io.h"

/*
 * TEST OPTIONS
 * Comment-in any features or peripherals
 * that you want to test. Any combination
 * of features is intended to work together.
 */
#define __LED_TEST
#define __UART_TEST
#define __LCD_TEST

#ifdef __LED_TEST
#include "drivers/led.h"
#define LED_PIN (5)
#endif // __LED_TEST
#ifdef __UART_TEST
#include "drivers/uart.h"
#endif // __UART_TEST
#ifdef __LCD_TEST
#include "drivers/lcd.h"
#endif // __LCD_TEST

#define DELAY_COUNT (500) // Loop interval in milliseconds

int main(void)
{
    // clang-format off
    #ifdef __LED_TEST
    led_init();
    #endif // __LED_TEST

    #ifdef __UART_TEST
    uart_init();

    #ifdef __LCD_TEST
    static uint8_t lcd_char_idx = 0;
    #endif // UART/LCD_TEST
    
    #endif // __UART_TEST

    #ifdef __LCD_TEST
    lcd_init();
    lcd_transmit_string("Hello World!", sizeof("Hello World!") - 1);
    lcd_set_cursor(1, 0);
    #endif // __LCD_TEST

    while (1) {
	#ifdef __UART_TEST
        while (uart_data_available()) {
            char c = uart_read_char();
            uart_send_char(c);
            uart_transmit("\r\n");

	    #ifdef __LCD_TEST
            lcd_send_data(c);
            if (lcd_char_idx == 15) {
                lcd_char_idx = 0;
                lcd_set_cursor(1, 0);
            } else {
                lcd_char_idx++;
            }
	    #endif // UART/LCD_TEST
        }
	#endif // __UART_TEST

        #ifdef __LED_TEST
        led_toggle();
	#endif // __LED_TEST

	#ifdef __UART_TEST
        uart_transmit("Sending test string!\r\n");
	#endif // __UART_TEST
        // clang-format on
        delay_ms(DELAY_COUNT);
    }
}
