#include <stm32l476xx.h>
#include "drivers/io.h"

void io_enable_clock(io_port port)
{
    RCC->AHB2ENR |= 0X1UL << port;
}

void io_set_mode(io_port port, uint8_t pin, io_mode mode)
{
    // TODO: Assert pin within [15:0] & Mode 0-3

    // GPIO_TypeDef defined in stm32l476xx.h
    // Stride through memory to find correct GPIO port base address
    GPIO_TypeDef *gpio = (GPIO_TypeDef *)(GPIOA_BASE + (GPIOB_BASE - GPIOA_BASE) * port);

    gpio->MODER &= ~(0x3UL << (pin << 1)); // Clear 2 bits of MODE(pin)
    gpio->MODER |= (mode << (pin << 1)); // Set desired mode to MODE(pin)
}

void io_set_otype(io_port port, uint8_t pin, io_otype otype)
{
    // TODO: Assert pin within [15:0]
    GPIO_TypeDef *gpio = (GPIO_TypeDef *)(GPIOA_BASE + (GPIOB_BASE - GPIOA_BASE) * port);

    gpio->OTYPER &= ~(0x1UL << pin); // Clear OTYPE(pin)
    gpio->OTYPER |= (otype << pin); // Set otype to OTYPE(pin)
}
void io_set_ospeed(io_port port, uint8_t pin, io_ospeed ospeed)
{
    // TODO: Assert pin within [15:0]
    GPIO_TypeDef *gpio = (GPIO_TypeDef *)(GPIOA_BASE + (GPIOB_BASE - GPIOA_BASE) * port);

    gpio->OSPEEDR &= ~(0x3UL << (pin << 1)); // Clear 2 bits of OSPEED(pin)
    gpio->OSPEEDR |= (ospeed << (pin << 1)); // Set desired ospeed to OSPEED(pin)
}
