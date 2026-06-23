#include "stm32l476xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include <string.h>
#include <string.h>
#include "common/utils.h"
#include "drivers/io.h"

/* TEST OPTIONS
 * Comment-in any features or peripherals
 * that you want to test. Any combination
 * of features is intended to work together.
 */
#define LED_TEST
#define UART_TEST
// #define LCD_TEST
// #define SPI_OLED_TEST

/* LED_TEST ****************************************************/
#ifdef LED_TEST
#include "drivers/led.h"
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
#endif // LED_TEST

/* UART_TEST ***************************************************/
#ifdef UART_TEST
#include "drivers/uart.h"

static void uart_transmit_task(void *arg)
{
    (void)arg;
    for (;;) {
        uart_transmit("Sending test string!\r\n");
        vTaskDelay(1000);
    }
}
#endif // UART_TEST

/* LCD_TEST ****************************************************/
#ifdef LCD_TEST
#include "drivers/lcd.h"
#endif // LCD_TEST

/* SPI_OLED_TEST ***********************************************/
#ifdef SPI_OLED_TEST
#include "drivers/spi1.h"
#include "drivers/oled.h"
#endif // SPI_OLED_TEST

int main(void)
{

    // clang-format off
    #ifdef LED_TEST
    led_init();
    xTaskCreate(blink_task, "b", 256, NULL, 1, NULL);
    #endif // LED_TEST

    #ifdef UART_TEST
    uart_init();
    xTaskCreate(uart_transmit_task, "b", 256, NULL, 1, NULL);
    #endif // UART_TEST
    // clang-format on

    /* Enable fault handlers */
    SCB->SHCSR |= SCB_SHCSR_USGFAULTENA_Msk | SCB_SHCSR_BUSFAULTENA_Msk | SCB_SHCSR_MEMFAULTENA_Msk;

    vTaskStartScheduler();
    for (;;) { }
}

void vApplicationIdleHook(void) { } // IDLE TASK

void vApplicationMallocFailedHook(void)
{
    // TODO: ASSERT(0)
    for (;;) {
        GPIOA->ODR ^= (1U << 5);
        for (volatile uint32_t d = 0; d < 200000; d++) { }
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
