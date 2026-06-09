

#include <stdint.h>
#include "stm32l476xx.h"
#include "common/utils.h"
#include "drivers/led.h"
#include "drivers/uart.h"
#include "drivers/io.h"
#include "drivers/lcd.h"
#include <string.h>

#define LED_PIN (5)
#define DELAY_COUNT (500) // blink interval in milliseconds

int main(void)
{

    led_init();
    uart_init();
    lcd_init();

    uint8_t lcd_char_idx = 0;
    lcd_transmit_string("Hello World!", sizeof("Hello World!") - 1);
    lcd_set_cursor(1, 0);

    while (1) {
        while (uart_data_available()) {
            char c = uart_read_char();
            uart_send_char(c);
            uart_transmit("\r\n");

	    lcd_send_data(c);
	    if(lcd_char_idx == 15) {
		    lcd_char_idx = 0;
		    lcd_set_cursor(1,0);
	    } else {
		    lcd_char_idx++;
	    }
        }

        led_toggle();

        uart_transmit("Sending test string!\r\n");

        delay_ms(DELAY_COUNT);
    }
}
