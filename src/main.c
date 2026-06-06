

#include <stdint.h>
#include "stm32l476xx.h"
#include "common/utils.h"
#include "drivers/led.h"

#define LED_PIN (5)
#define DELAY_COUNT (250) // blink interval in milliseconds

int main(void)
{

    // 1. Enable Clock for GPIOA (AHB2 peripheral clock enable register)
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

    led_init();
    while (1) {
        led_toggle();
        delay_ms(DELAY_COUNT);
    }
}
