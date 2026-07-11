#ifndef __UART_H_
#define __UART_H_

#include <stdint.h>

#define RX_BUF_SIZE 64
static inline uint32_t rx_next(uint32_t idx)
{
    return (idx + 1) % RX_BUF_SIZE;
}

void uart_start_tasks(void);
void uart_init(void);
void uart_send_char_polling(char c);
void uart_send_char_interrupt(char c);
void uart_transmit(const char *buf);
void uart_flush(void);
int uart_check_input_polling(char *out);
int uart_data_available(void);
char uart_read_char(void);
void uart_print_hex(const char *label, uint32_t val);
#endif // __UART_H_
