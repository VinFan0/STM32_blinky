#ifndef __LCD_H_
#define __LCD_H_

#include <stdint.h>

/*
 * LCD Pins on Nucleo-L476RG dev board
 * VSS -> GND
 * VDD -> 5V
 * V0 -> Pull-down to GND (2.2k ohm)
 * RS -> PC5
 * RW -> GND
 * E -> PC4
 * D0-D3 -> GND
 * D4-D7 -> PC0-PC3
 * A -> 5V
 * K -> GND
 */

#define LCD_PORT GPIOC
#define LCD_BUS_WIDTH 4
#define LCD_N 1
#define LCD_F 0
#define LCD_RS_PIN 5
#define LCD_E_PIN 4

void lcd_init(void);
void lcd_gpio_init(void);
void lcd_send_command(uint8_t com);
void lcd_send_data(char c);
void lcd_put_nibble(char nibble);
void lcd_pulse(void);
void lcd_transmit_string(char *str, uint8_t str_len);
void lcd_set_cursor(uint8_t line, uint8_t col);
void lcd_clear_line(uint8_t line);

#endif // __LCD_H_
