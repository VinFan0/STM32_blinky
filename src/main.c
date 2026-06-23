#include "stm32l476xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include <string.h>
#include <string.h>
#include "common/utils.h"
#include "drivers/io.h"
#include "drivers/led.h"
#include "drivers/uart.h"

#define LED_PIN 5U

static void blink_task(void *arg)
{
    (void)arg;
    /* Direct register access — no clock_init needed, GPIOA already enabled */
    for (;;) {
        GPIOA->ODR ^= (1U << LED_PIN);
        vTaskDelay(250);
    }
}

static void uart_transmit_task(void *arg)
{
    (void)arg;
    for (;;) {
	uart_transmit("Sending test string!\r\n");
	vTaskDelay(1000);
    }
}

int main(void)
{

    led_init();

    uart_init();

    /* Enable fault handlers */
    SCB->SHCSR |= SCB_SHCSR_USGFAULTENA_Msk | SCB_SHCSR_BUSFAULTENA_Msk | SCB_SHCSR_MEMFAULTENA_Msk;

    xTaskCreate(blink_task, "b", 256, NULL, 1, NULL);
    xTaskCreate(uart_transmit_task, "b", 256, NULL, 1, NULL);

    vTaskStartScheduler();
    for (;;) { }
}

void vApplicationIdleHook(void) { } // IDLE TASK

void vApplicationMallocFailedHook(void)
{
    // TODO: ASSERT(0)
    for (;;) {
        GPIOA->ODR ^= (1U << LED_PIN);
        for (volatile uint32_t d = 0; d < 50000; d++) { }
    }
}
void vApplicationStackOverflowHook(TaskHandle_t t, char *n)
{
    (void)t;
    (void)n;
    // TODO: ASSERT(0)
    for (;;) {
	    GPIOA->ODR ^= (1U << LED_PIN);
	    for(volatile uint32_t d=0; d<300000; d++) { }
    }
}
void HardFault_Handler(void)
{
    // TODO: ASSERT(0)
    for (;;) {
	    GPIOA->ODR ^= (1U << LED_PIN);
	    for(volatile uint32_t d=0; d<400000; d++) { }
    }
}
void MemManage_Handler(void)
{
    // TODO: ASSERT(0)
    for (;;) {
	    GPIOA->ODR ^= (1U << LED_PIN);
	    for(volatile uint32_t d=0; d<500000; d++) { }
    }
}
void BusFault_Handler(void)
{
    // TODO: ASSERT(0)
    for (;;) {
	    GPIOA->ODR ^= (1U << LED_PIN);
	    for(volatile uint32_t d=0; d<600000; d++) { }
    }
}
void UsageFault_Handler(void)
{
    // TODO: ASSERT(0)
    for (;;) {
	    GPIOA->ODR ^= (1U << LED_PIN);
	    for(volatile uint32_t d=0; d<700000; d++) { }
    }
}
