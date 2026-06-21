#include "stm32l476xx.h"
#include "FreeRTOS.h"
#include "task.h"

#define LED_PIN 5U

static void blink_task(void *arg)
{
    (void)arg;
    /* Direct register access — no clock_init needed, GPIOA already enabled */
    for (;;) {
        GPIOA->ODR ^= (1U << LED_PIN);
        vTaskDelay(500);
    }
}

void vApplicationIdleHook(void) { }
void vApplicationMallocFailedHook(void)
{
    for (;;) { }
}
void vApplicationStackOverflowHook(TaskHandle_t t, char *n)
{
    (void)t;
    (void)n;
    for (;;) { }
}
void HardFault_Handler(void)
{
    for (;;) {
        GPIOA->ODR ^= (1U << LED_PIN);
        for (volatile uint32_t d = 0; d < 100000; d++) { }
    }
}
void MemManage_Handler(void)
{
    for (;;) {
        GPIOA->ODR ^= (1U << LED_PIN);
        for (volatile uint32_t d = 0; d < 30000; d++) { }
    }
}
void BusFault_Handler(void)
{
    for (;;) {
        GPIOA->ODR ^= (1U << LED_PIN);
        for (volatile uint32_t d = 0; d < 150000; d++) { }
    }
}
void UsageFault_Handler(void)
{
    for (;;) {
        GPIOA->ODR ^= (1U << LED_PIN);
        for (volatile uint32_t d = 0; d < 300000; d++) { }
    }
}

int main(void)
{
    /* Enable GPIOA clock */
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
    (void)RCC->AHB2ENR;

    /* Enable fault handlers */
    SCB->SHCSR |= SCB_SHCSR_USGFAULTENA_Msk | SCB_SHCSR_BUSFAULTENA_Msk | SCB_SHCSR_MEMFAULTENA_Msk;

    /* PA5 output */
    GPIOA->MODER = (GPIOA->MODER & ~(3U << (LED_PIN * 2))) | (1U << (LED_PIN * 2));
    GPIOA->OTYPER &= ~(1U << LED_PIN);
    GPIOA->PUPDR &= ~(3U << (LED_PIN * 2));
    GPIOA->OSPEEDR &= ~(3U << (LED_PIN * 2));

    xTaskCreate(blink_task, "b", 256, NULL, 1, NULL);

    vTaskStartScheduler();
    for (;;) { }
}
