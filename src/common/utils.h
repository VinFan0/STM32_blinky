#ifndef __UTILS_H
#define __UTILS_H

#include <stdint.h>

void delay_ms(volatile uint32_t ms);
void HardFault_Handler(void);
#endif // __UTILS_H
