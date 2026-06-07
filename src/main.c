

#include <stdint.h>
#include "stm32l476xx.h"
#include "common/utils.h"
#include "drivers/led.h"
#include "drivers/uart.h"
#include "drivers/io.h"

#define LED_PIN (5)
#define DELAY_COUNT (250) // blink interval in milliseconds

int main(void)
{

    led_init();
    uart_init();
    while (1) {
        led_toggle();

        uart_transmit("Sending test string!\r\n");

        delay_ms(DELAY_COUNT);
    }
}
