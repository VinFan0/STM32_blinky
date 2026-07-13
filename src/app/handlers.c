#include "stm32l476xx.h"
#include "FreeRTOS.h"
#include "task.h"

void vApplicationIdleHook(void) { } // IDLE TASK

void vApplicationMallocFailedHook(void)
{
    // TODO: ASSERT(0)
    for (;;) {
        GPIOA->ODR ^= (1U << 5);
        for (volatile uint32_t d = 0; d < 20000; d++) { }
    }
}
void vApplicationStackOverflowHook(TaskHandle_t t, char *n)
{
    (void)t;
    (void)n;
    // TODO: ASSERT(0)
    for (;;) {
        GPIOA->ODR ^= (1U << 5);
        for (volatile uint32_t d = 0; d < 300000; d++) { }
    }
}
void HardFault_Handler(void)
{
    // TODO: ASSERT(0)
    for (;;) {
        GPIOA->ODR ^= (1U << 5);
        for (volatile uint32_t d = 0; d < 400000; d++) { }
    }
}
void MemManage_Handler(void)
{
    // TODO: ASSERT(0)
    for (;;) {
        GPIOA->ODR ^= (1U << 5);
        for (volatile uint32_t d = 0; d < 500000; d++) { }
    }
}
void BusFault_Handler(void)
{
    // TODO: ASSERT(0)
    for (;;) {
        GPIOA->ODR ^= (1U << 5);
        for (volatile uint32_t d = 0; d < 600000; d++) { }
    }
}
void UsageFault_Handler(void)
{
    // TODO: ASSERT(0)
    for (;;) {
        GPIOA->ODR ^= (1U << 5);
        for (volatile uint32_t d = 0; d < 700000; d++) { }
    }
}
