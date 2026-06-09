
#include "utils.h"
#include "drivers/led.h"

/*
 * Function:	delay_ms
 * Input: 	volatile uint32_t ms --> time to delay in milliseconds
 * Output: 	none
 * Description:
 * 	Utilize inline assembly instructions for precise timing of desired
 * 	milliseconds. Configured for default 4 MHz clock on Nucleo-L476RG
 *
 * TODO: Support different clock speeds
 */
void delay_ms(volatile uint32_t ms)
{
    // At 4 MHz, 1 ms is 4,000 clock cycles
    // On Cortex-M 'subs' + 'bne' take 3 cylces per iteration
    // (4000 cycles) / (3 cycles/loop) = 1333 iterations per ms
    uint32_t cycles_per_ms = 1333;

    while (ms--) {
        uint32_t count = cycles_per_ms;
        __asm__ volatile("1: subs %0, %0, #1 \n"
                         "       bne 1b \n"
                         : "+r"(count));
    }
}

uint8_t my_strlen(const char *s)
{
    uint8_t i = 0;
    while (s[i] != '\0')
        i++;
    return i;
}
