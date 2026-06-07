#ifndef __UART_H_
#define __UART_H_

void uart_init(void);
void uart_send_char(char c);
void uart_transmit(char *buf);
void uart_flush(void);

#endif // __UART_H_
