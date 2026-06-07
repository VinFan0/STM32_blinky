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

void io_set_pupdr(io_port port, uint8_t pin, io_pupd pupd)
{
    // TODO: Assert pin within [15:0]
    GPIO_TypeDef *gpio = (GPIO_TypeDef *)(GPIOA_BASE + (GPIOB_BASE - GPIOA_BASE) * port);

    gpio->PUPDR &= ~(0x3UL << (pin << 1));
    gpio->PUPDR |= (pupd << (pin << 1));
}

void io_set_afr(io_port port, uint8_t pin, io_afr afr)
{

    // TODO: Assert pin within [15:0]
    GPIO_TypeDef *gpio = (GPIO_TypeDef *)(GPIOA_BASE + (GPIOB_BASE - GPIOA_BASE) * port);

    if (pin <= 7) {
        gpio->AFR[0] &= ~(0x15UL << (pin << 2)); // Clear 4 bits of AFRL(pin)
        gpio->AFR[0] |= (afr << (pin << 2)); // Set desired afr to AFRL(pin)
    } else if (pin <= 15) {
        gpio->AFR[1] &= ~(0x15UL << (pin << 2)); // Clear 4 bits of AFRH(pin)
        gpio->AFR[1] |= (afr << (pin << 2)); // Set desired afr to AFRH(pin)
    }
}

void io_set_analag_switch_control(io_port port, uint8_t pin, io_ascr ascr)
{

    /* NOTE: This bit must be set prior to ADC conversion.
     * ***********************************************************
     * Only the IO which connect to the ADC input are effective. Other
     * IOs must keep their reset value.
     */

    // TODO: Assert pin within [15:0]
    // TODO: Assert port within A to H
    GPIO_TypeDef *gpio = (GPIO_TypeDef *)(GPIOA_BASE + (GPIOB_BASE - GPIOA_BASE) * port);

    gpio->ASCR &= ~(0x1UL << pin); // Clear bit of ASCR(pin)
    gpio->ASCR |= (ascr << pin); // Set desired ascr to ASCR(pin)
}
