
#include "stm32l476xx.h"
#include "drivers/led.h"
#include "drivers/io.h"
#include "FreeRTOS.h"
#include "task.h"

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

void led_create_tasks(void)
{
    xTaskCreate(blink_task, "blink", 256, NULL, 1, NULL);
}

void led_init(void)
{
    // On the Nucleo-L476RG dev board, the built in LED is on PA5
    //
    // Enable GPIO Port A Clock
    io_enable_clock(IO_PORT_A);

    // Set PA5 (Built in LED) to General purpose Output (01)
    io_set_mode(IO_PORT_A, 5, IO_MODE_OUTPUT);

    // OPTIONAL: Configure as Push-Pull (default is Push-Pull)
    io_set_otype(IO_PORT_A, 5, IO_OTYPE_PP);

    // OPTIONAL: Set Output speed to High
    io_set_ospeed(IO_PORT_A, 5, IO_OSPEED_LOW);
}

void led_toggle(void)
{

    // Toggle LED
    GPIOA->ODR ^= (1 << 5);
}
