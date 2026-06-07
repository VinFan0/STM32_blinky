#include "stm32l476xx.h"
#include "drivers/uart.h"
#include "drivers/io.h"

/* Nucleo-L476RG ST-Link virtual COM connected to USART2
 * TX -> PA2
 * RX -> PA3
 * Both AF7
 */

/* USART2 Init Workflow
 * Enable port A and USART2 clocks
 * Configure TX and RX pins
 * - Alternate Function Mode
 * - AF7
 * - Set TX output speed high
 * - Disable PUPDR
 * Configure USART2
 * - Disable USART pre-config
 * - Set buad rate: PCLK1 defaults to 4MHz post-reset
 *   - BRR = PCLK1 / baud (4MHz / 115200 = ~35)
 * - USART Defaults (8 data bits, no parity, 1 stop bit)
 * - Enable TX and RS, then enable USART2
 * - Wait until USART is ready to transmit
 */
void uart_init(void)
{
    // Configure TX and RX pins
    io_enable_clock(IO_PORT_A);
    RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;

    io_set_mode(IO_PORT_A, 2, IO_MODE_AF);
    io_set_mode(IO_PORT_A, 3, IO_MODE_AF);

    io_set_afr(IO_PORT_A, 2, IO_AF7);

    io_set_ospeed(IO_PORT_A, 2, IO_OSPEED_HIGH);

    io_set_pupdr(IO_PORT_A, 2, IO_PUPD_NONE);
    io_set_pupdr(IO_PORT_A, 3, IO_PUPD_NONE);

    // Configure USART2
    USART2->CR1 &= ~USART_CR1_UE;

    USART2->BRR = 35; // BRR = PCLK1 / baud (4MHz / 115200 = ~35)

    USART2->CR1 |= USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
}

void uart_send_char(char c)
{
    while (!(USART2->ISR & USART_ISR_TXE))
        ;
    USART2->TDR = c;
}

void uart_transmit(char *str)
{
    while (*str) {
        uart_send_char(*str++);
    }
}

void uart_flush(void)
{
    while (!(USART2->ISR & USART_ISR_TC))
        ;
}
