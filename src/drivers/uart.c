#include <stm32l476xx.h>
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
    io_set_afr(IO_PORT_A, 3, IO_AF7);

    io_set_ospeed(IO_PORT_A, 2, IO_OSPEED_HIGH);

    io_set_pupdr(IO_PORT_A, 2, IO_PUPD_NONE);
    io_set_pupdr(IO_PORT_A, 3, IO_PUPD_NONE);

    // Configure USART2
    USART2->CR1 &= ~USART_CR1_UE;

    USART2->BRR = 35; // BRR = PCLK1 / baud (4MHz / 115200 = ~35)

    USART2->CR1 |= USART_CR1_TE | USART_CR1_RE | USART_CR1_UE | USART_CR1_RXNEIE;

    USART2->ISR |= USART_ICR_ORECF | USART_ICR_FECF | USART_ICR_NCF | USART_ICR_PECF;

    NVIC_SetPriority(USART2_IRQn, 1);
    NVIC_EnableIRQ(USART2_IRQn);

    if (USART2->ISR & USART_ISR_RXNE)
        (void)USART2->RDR;

    while (!(USART2->ISR & USART_ISR_TEACK))
        ;
    while (!(USART2->ISR & USART_ISR_REACK))
        ;
}

void uart_send_char(char c)
{
    while (!(USART2->ISR & USART_ISR_TXE))
        ;
    USART2->TDR = c;
}

void uart_transmit(const char *str)
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

int uart_check_input_polling(char *out)
{
    if (!(USART2->ISR & USART_ISR_RXNE))
        return 0;
    *out = (char)(USART2->RDR & 0xFF);
    return 1;
}

int uart_data_available(void)
{
    return rx_head != rx_tail;
}

char uart_read_char(void)
{
    char c = rx_buf[rx_tail];
    rx_tail = rx_next(rx_tail);
    return c;
}

void USART2_IRQHandler(void)
{
    if (USART2->ISR & USART_ISR_RXNE) {
        char c = (char)(USART2->RDR & 0xFF);

        uint32_t next = rx_next(rx_head);
        if (next != rx_tail) {
            rx_buf[rx_head] = c;
            rx_head = next;
        }
    }

    if (USART2->ISR & USART_ISR_ORE) {
        USART2->ICR |= USART_ICR_ORECF;
    }
}

void uart_print_hex(const char *label, uint32_t val)
{
    char buf[12];
    uart_transmit(label);
    uart_transmit(": 0x");

    for (int i = 7; i >= 0; i--) {
        uint8_t nibble = (val >> (i << 2)) & 0xF;
        buf[7 - i] = nibble < 10 ? '0' + nibble : 'A' + nibble - 10;
    }
    buf[8] = '\r';
    buf[9] = '\n';
    buf[10] = '\0';
    uart_transmit(buf);
}
