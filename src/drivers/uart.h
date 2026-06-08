#ifndef __UART_H_
#define __UART_H_

#include <stdint.h>

#define RX_BUF_SIZE 64

static volatile char rx_buf[RX_BUF_SIZE];
static volatile uint32_t rx_head = 0;
static volatile uint32_t rx_tail = 0;

void uart_init(void);
void uart_send_char(char c);
void uart_transmit(char *buf);
void uart_flush(void);
int uart_check_input_polling(char *out);
static inline uint32_t rx_next(uint32_t idx)
{
    return (idx + 1) % RX_BUF_SIZE;
}
int uart_data_available(void);
char uart_read_char(void);
void USAR2_IRQHandler(void);
#endif // __UART_H_
