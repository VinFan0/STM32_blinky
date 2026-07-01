#ifndef __OLED_H_
#define __OLED_H_

#include <stdint.h>
#include "app/oled_fonts/gfxfont.h"
/*
 * Extra pins required to drive PMOD OLED device,
 * in addition to the SPI1 pins.
 *
 * The OLED display has a specific power-up and
 * power-down sequence to ensure the longevity of
 * the device.
 *
 * Pins VDDC and VBATC are connected to FETs that
 * control the display's two power supplies.
 * VDDC: control toggles power to the logic of the display
 * VBATC: control toggles power to teh OLED display itself
 *
 * The two pins have pull-up resistors that turn off
 * their respective power supplies when they're not being
 * driven low to turn on the power supply.
 */
#define OLED_VDDC 11
#define OLED_VBATC 12

#define OLED_VDDC_LOW() GPIOA->ODR &= ~(1U << OLED_VDDC)
#define OLED_VDDC_HIGH() GPIOA->ODR |= (1U << OLED_VDDC)
#define OLED_VBATC_LOW() GPIOA->ODR &= ~(1U << OLED_VBATC)
#define OLED_VBATC_HIGH() GPIOA->ODR |= (1U << OLED_VBATC)

extern uint8_t oled_buf[512];

void oled_init(void);
void oled_gpio_init(void);
void oled_send_command(uint8_t cmd);
void oled_send_data(uint8_t data);
void oled_flush(void);
void oled_clear(void);
void oled_set_pixel(int16_t x, int16_t y, bool on);
void oled_draw_line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, bool on);
void oled_draw_rectangle(int16_t c1x, int16_t c1y, int16_t c2x, int16_t c2y, bool on);
void oled_draw_circle(int16_t x0, int16_t y0, int16_t radius, bool on);
void oled_draw_char(int16_t x, int16_t y, unsigned char c, const GFXfont *font, bool on);
void oled_draw_string(int16_t x, int16_t y, const char *str, const GFXfont *font, bool on);
void oled_scroll_start(uint8_t start_page, uint8_t end_page, uint8_t frame_interval, bool scroll_left);
void oled_scroll_stop(void);
#endif // __OLED_H_
