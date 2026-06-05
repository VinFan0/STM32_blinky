
#include "stm32l476xx.h"
#include "led.h"
#include "common/utils.h"

void led_init(void) {
	// Enable GPIO Port A Clock
	// Set PA5 (Built in LED) to General purpose Output (01)
	GPIOA->MODER &= ~GPIO_MODER_MODE5;
	GPIOA->MODER |= GPIO_MODER_MODE5_0;

	// OPTIONAL: Configure as Push-Pull (default is Push-Pull)
	GPIOA->OTYPER &= ~GPIO_OTYPER_OT5;

	// OPTIONAL: Set Output speed to High
	GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED5;
}

void led_toggle(void) {

	// Toggle LED 
	GPIOA->ODR ^= (1 << 5);
}
