

#include <stdint.h>
#include "stm32l476xx.h"
#include "common/utils.h"
#include "drivers/led.h"

#define LED_PIN (5)
#define DELAY_COUNT (500) // blink interval in milliseconds

int main(void)
{

    led_init();
    while (1) {
        led_toggle();
        delay_ms(DELAY_COUNT);
    }
}
