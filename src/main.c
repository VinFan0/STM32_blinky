

#include <stdint.h>
#include "stm32l476xx.h"
#include "common/utils.h"
#include "drivers/led.h"
#include "drivers/uart.h"
#include "drivers/io.h"

#define LED_PIN (5)
#define DELAY_COUNT (500) // blink interval in milliseconds

int main(void)
{

    led_init();
    uart_init();

    while (1) {
        while (uart_data_available()) {
            char c = uart_read_char();
            uart_send_char(c);
            uart_transmit("\r\n");
        }

        led_toggle();

        uart_transmit("Sending test string!\r\n");

        delay_ms(DELAY_COUNT);
    }
}
