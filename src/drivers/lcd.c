#include "stm32l476xx.h"
#include "drivers/lcd.h"
#include "drivers/io.h"
#include "common/utils.h"

/*
 * LCD Init Flowchart:
 *
 * N: Number of lines 0 = 1 line; 1 = 2 lines
 * F: Font size 0 = 5x8 dots; 1 = 5x11 dots
 *
 * Wait for >= 30 ms
 * Send command `0x30`
 * Wait for >= 4.1 ms
 * Send command `0x30`
 * Wait for >= 160 us
 * Send command `0x30`
 * Wait for >= 160 us
 *****************************
 * Does data bus have 8 bits?
 * Yes:
 * Send command `0b0011NF**`
 * Wait for >= 40 us
 *
 * No: Send command `0x02` to switch to 4 bit mode
 * Wait for >= 40 us
 * Send command `0b0010NF**`
 * Wait for >= 40 us
 *****************************
 * Send command `0x01` to clear display
 * Wait for >= 1.5 ms
 * Send command `0x06` to set entry mode
 * Wait for >= 40 us
 * Send command `0x0F to control display ON/OFF
 * Wait for >= 40 us
 */
void lcd_init(void)
{

    lcd_gpio_init(); // Config LCD GPIO Pins

    delay_ms(30);
    lcd_send_command(0x30); // 0b00110000
    delay_ms(5);
    lcd_send_command(0x30); // 0b00110000
    delay_ms(1);
    lcd_send_command(0x30); // 0b00110000
    delay_ms(1);

    if (LCD_BUS_WIDTH == 8) {
        lcd_send_command(0b00110000 ^ ((LCD_N << 3) | (LCD_F << 2))); // N: 2 lines; F: 5x8 dot font
        delay_ms(1);
    } else if (LCD_BUS_WIDTH == 4) {
        lcd_send_command(0x02); // 4-bit mode
        delay_ms(1);
        lcd_send_command(0b00100000 ^ ((LCD_N << 3) | (LCD_F << 2))); // N: 2 lines; F: 5x8 dot font
        delay_ms(1);
    } else
        while (1) { }

    lcd_send_command(0x01); // CLS, DDRAM to 0
    delay_ms(2);
    lcd_send_command(0x06); // Entry Mode: Increment mode
    delay_ms(1);
    lcd_send_command(0x0E); // 0b0000_1DCB D: Display on; C: Cursor on; B: Blink on
    delay_ms(1);
}

void lcd_gpio_init(void)
{
    io_enable_clock(IO_PORT_C);
    for (int i = 0; i <= 5; i++) {
        io_set_mode(IO_PORT_C, i, IO_MODE_OUTPUT);
        io_set_otype(IO_PORT_C, i, IO_OTYPE_PP);
        io_set_pupdr(IO_PORT_C, i, IO_PUPD_NONE);
    }
}

void lcd_send_command(uint8_t com)
{
    LCD_PORT->ODR &= ~(1 << LCD_RS_PIN);
    lcd_put_nibble(com >> 4);
    lcd_pulse();
    lcd_put_nibble(com & 0xF);
    lcd_pulse();
    LCD_PORT->ODR |= 1 << LCD_RS_PIN;
}

void lcd_send_data(char c)
{
    lcd_put_nibble(c >> 4);
    lcd_pulse();
    lcd_put_nibble(c & 0xF);
    lcd_pulse();
}

void lcd_put_nibble(char nibble)
{
    LCD_PORT->ODR = (LCD_PORT->ODR & ~0xF) | (nibble & 0xF);
}

void lcd_pulse(void)
{
    LCD_PORT->ODR |= (1 << LCD_E_PIN);
    delay_ms(1);
    LCD_PORT->ODR &= ~(1 << LCD_E_PIN);
    delay_ms(1);
}

void lcd_transmit_string(char *str, uint8_t str_len)
{
    if (str_len <= 16) {
        for (int i = 0; i < str_len; i++) {
            lcd_send_data(str[i]);
        }
    }
}

void lcd_set_cursor(uint8_t line, uint8_t col)
{
    uint8_t line_offsets[] = { 0x00, 0x40 };
    lcd_send_command(0x80 | (line_offsets[line] + col));
}

void lcd_clear_line(uint8_t line)
{
    lcd_set_cursor(line, 0);
    for (int i = 0; i < 16; i++) {
        lcd_send_data(' ');
    }
    lcd_set_cursor(line, 0);
}
