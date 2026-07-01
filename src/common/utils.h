#ifndef __UTILS_H
#define __UTILS_H

#include <stdint.h>

void delay_ms(volatile uint32_t ms);
uint8_t my_strlen(const char *s);
int16_t abs16(int16_t x);
#endif // __UTILS_H
