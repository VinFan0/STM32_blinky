#ifndef __LCD_H_
#define __LCD_H_

#include <stdint.h>

void lcd_init(void);
void lcd_gpio_init(void);
void lcd_send_command(uint8_t com);
void lcd_send_data(char c);
void lcd_put_nibble(char nibble);
void lcd_pulse(void);
void lcd_transmit_string(char *str, uint8_t str_len);
void lcd_set_cursor(uint8_t line, uint8_t col);

#endif // __LCD_H_
