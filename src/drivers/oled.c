#include <string.h>
#include "stm32l476xx.h"
#include "drivers/oled.h"
#include "io.h"
#include "common/utils.h"
#include "drivers/spi1.h"
#include "FreeRTOS.h"
#include "task.h"

uint8_t oled_buf[512];

static void oled_task(void *arg)
{
    (void)arg;
    static bool inverted = 1;
    for (;;) {
        for (int i = 0; i <= 8; i++) {
            oled_draw_line(0 + (i << 1), 0 + (i << 1), 127 - (i << 1), 0 + (i << 1), 1);
            if (i == 8) {
                oled_clear();
                if (inverted)
                    inverted = 0;
                else
                    inverted = 1;
                oled_invert(inverted);
                break;
            }
            oled_draw_line(0 + (i << 1), 31 - (i << 1), 127 - (i << 1), 31 - (i << 1), 1);
            oled_draw_line(0 + (i << 1), 0 + (i << 1), 0 + (i << 1), 31 - (i << 1), 1);
            oled_draw_line(127 - (i << 1), 0 + (i << 1), 127 - (i << 1), 31 - (i << 1), 1);
            oled_flush_fromtask();
            vTaskDelay(500);
        }
        oled_flush_fromtask();
    }
}

void oled_create_tasks(void)
{
    xTaskCreate(oled_task, "oled", 256, NULL, 1, NULL);
}

void oled_init(void)
{
    spi1_init();
    oled_gpio_init();

    // Power UP Sequence
    OLED_VDDC_HIGH();
    OLED_VBATC_HIGH();

    OLED_VDDC_LOW();
    delay_ms(1);

    // Reset
    SPI1_RES_LOW();
    delay_ms(1);
    SPI1_RES_HIGH();
    delay_ms(1);

    oled_send_command(0xAE); // Display off

    oled_send_command(0xD5); // Set display clock
    oled_send_command(0x80);
    oled_send_command(0xA8); // Set mutilpex ratio
    oled_send_command(0x1F); // 32 rows
    oled_send_command(0xD3); // Display offset
    oled_send_command(0x00);
    oled_send_command(0x40); // Start line = 0
    oled_send_command(0x8D); // Charge pump
    oled_send_command(0x14); // Enable charge pump
    oled_send_command(0x20); // Memory addressing mode
    oled_send_command(0x00); // Horizontal addressing
    oled_send_command(0xA0); // Segment remap A0/A1 horizontal flip
    oled_send_command(0xC0); // COM scan direction C0/C8 vertial flip
    oled_send_command(0xDA); // COM pins
    oled_send_command(0x02); // 128x32 COM pin config
    oled_send_command(0x81); // Contrast
    oled_send_command(0xFF);
    oled_send_command(0xD9); // Pre-charge period
    oled_send_command(0xF1);
    oled_send_command(0xDB); // VCOMH deselect level
    oled_send_command(0x40);
    oled_send_command(0xA4); // Entire display on (use RAM)
    oled_send_command(0xA6); // Normal display (not inverted)

    oled_clear(); // Clear buffer to zeros
    oled_flush(); // Flush buffer to display

    OLED_VBATC_LOW();

    delay_ms(100);

    oled_send_command(0xAF); // Display on
}

void oled_gpio_init(void)
{
    io_set_mode(IO_PORT_A, OLED_VDDC, IO_MODE_OUTPUT);
    io_set_mode(IO_PORT_A, OLED_VBATC, IO_MODE_OUTPUT);

    io_set_otype(IO_PORT_A, OLED_VDDC, IO_OTYPE_PP);
    io_set_otype(IO_PORT_A, OLED_VBATC, IO_OTYPE_PP);

    io_set_ospeed(IO_PORT_A, OLED_VDDC, IO_OSPEED_HIGH);
    io_set_ospeed(IO_PORT_A, OLED_VBATC, IO_OSPEED_HIGH);

    io_set_pupdr(IO_PORT_A, OLED_VDDC, IO_PUPD_NONE);
    io_set_pupdr(IO_PORT_A, OLED_VBATC, IO_PUPD_NONE);
}

void oled_send_command(uint8_t cmd)
{
    SPI1_DC_CMD();
    SPI1_CS_LOW();
    spi1_send(cmd);
    SPI1_CS_HIGH();
}

void oled_send_data(uint8_t data)
{
    SPI1_DC_DAT();
    SPI1_CS_LOW();
    spi1_send(data);
    SPI1_CS_HIGH();
}

void oled_clear(void)
{
    for (int i = 0; i < (int)sizeof(oled_buf); i++) {
        oled_buf[i] = 0;
    }
}

void oled_flush(void)
{
    oled_send_command(0x21);
    oled_send_command(0x00);
    oled_send_command(0x7F);
    oled_send_command(0x22);
    oled_send_command(0x00);
    oled_send_command(0x03);

    SPI1_DC_DAT();
    SPI1_CS_LOW();
    for(uint16_t i=0; i<sizeof(oled_buf); i++) {
    	spi1_send(oled_buf[i]);
    }
    SPI1_CS_HIGH();
}

void oled_flush_fromtask(void)
{
    oled_send_command(0x21);
    oled_send_command(0x00);
    oled_send_command(0x7F);
    oled_send_command(0x22);
    oled_send_command(0x00);
    oled_send_command(0x03);

    SPI1_DC_DAT();
    SPI1_CS_LOW();
    spi1_send_stream(oled_buf, sizeof(oled_buf));
    SPI1_CS_HIGH();
}

void oled_set_pixel(int16_t x, int16_t y, bool on)
{
    // Each byte covers 8 vertical pixels (one "page")
    if ((x >= 0 && x < 128) && (y >= 0 && y < 32)) {
        int16_t idx = x + ((y >> 3) << 7);
        if (on) {
            oled_buf[idx] |= (1U << (y & 0x7)); // y % 8
        } else {
            oled_buf[idx] &= ~(1U << (y & 0x7));
        }
    }
}

void oled_draw_line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, bool on)
{
    int16_t dx = abs16(x1 - x0);
    int16_t dy = abs16(y1 - y0);
    int16_t sx = (x0 < x1) ? 1 : -1;
    int16_t sy = (y0 < y1) ? 1 : -1;
    int16_t err = dx - dy;

    while (1) {
        oled_set_pixel(x0, y0, on);

        if (x0 == x1 && y0 == y1)
            break;

        int16_t e2 = err << 1;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void oled_draw_rectangle(int16_t c1x, int16_t c1y, int16_t c2x, int16_t c2y, bool on)
{
    int16_t minx = 0;
    int16_t maxx = 0;
    int16_t miny = 0;
    int16_t maxy = 0;
    // Find min and max x (minx, maxx)
    if (c1x < c2x) {
        minx = c1x;
        maxx = c2x;
    } else if (c1x > c2x) {
        minx = c2x;
        maxx = c1x;
    } else if (c1x == c2x) {
        minx = c1x;
        maxx = minx;
    }
    // Find min and max y (miny, maxy)
    if (c1y < c2y) {
        miny = c1y;
        maxy = c2y;
    } else if (c1y > c2y) {
        miny = c2y;
        maxy = c1y;
    } else if (c1y == c2y) {
        miny = c1y;
        maxy = miny;
    }
    // Loop from miny to maxy
    for (int16_t idxy = miny; idxy <= maxy; idxy++) {
        // Loop from minx to maxx
        for (int16_t idxx = minx; idxx <= maxx; idxx++) {
            // Set pixel to <on> state
            oled_set_pixel(idxx, idxy, on);
            // Endloop
        }
        // Endloop
    }
}

// Bresenham’s Circle Algorithm
void oled_draw_circle(int16_t x0, int16_t y0, int16_t r, bool on)
{
    int8_t f = 1 - r;
    int8_t ddF_x = 1;
    int8_t ddF_y = -2 * r;
    int8_t x = 0;
    int8_t y = r;

    oled_set_pixel(x0, y0 + r, on);
    oled_set_pixel(x0, y0 - r, on);
    oled_set_pixel(x0 + r, y0, on);
    oled_set_pixel(x0 - r, y0, on);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        oled_set_pixel(x0 + x, y0 + y, on);
        oled_set_pixel(x0 - x, y0 + y, on);
        oled_set_pixel(x0 + x, y0 - y, on);
        oled_set_pixel(x0 - x, y0 - y, on);
        oled_set_pixel(x0 + y, y0 + x, on);
        oled_set_pixel(x0 - y, y0 + x, on);
        oled_set_pixel(x0 + y, y0 - x, on);
        oled_set_pixel(x0 - y, y0 - x, on);
    }
}

void oled_draw_char(int16_t x, int16_t y, unsigned char c, const GFXfont *font, bool on)
{
    if (c < font->first || c > font->last) {
        return;
    }

    const GFXglyph *glyph = &font->glyph[c - font->first];
    const uint8_t *bitmap = font->bitmap;

    uint16_t bo = glyph->bitmapOffset;
    uint8_t w = glyph->width;
    uint8_t h = glyph->height;
    int8_t xo = glyph->xOffset;
    int8_t yo = glyph->yOffset;

    uint8_t bit = 0;
    uint8_t byte = 0;

    for (uint8_t yy = 0; yy < h; yy++) {
        for (uint8_t xx = 0; xx < w; xx++) {
            if (!(bit & 7)) {
                byte = bitmap[bo++];
            }
            bit++;

            if (byte & 0x80) {
                oled_set_pixel(x + xo + xx, y + yo + yy, on);
            }
            byte <<= 1;
        }
    }
}

void oled_draw_string(int16_t x, int16_t y, const char *str, const GFXfont *font, bool on)
{
    int16_t cx = x;
    int16_t cy = y;

    while (*str) {
        char c = *str++;

        if (c == '\n') {
            cx = x;
            cy += font->yAdvance;
            continue;
        }

        if (c < font->first || c > font->last) {
            continue;
        }

        oled_draw_char(cx, cy, c, font, on);
        cx += font->glyph[c - font->first].xAdvance;
    }
}

void oled_scroll_start(uint8_t start_page, uint8_t end_page, uint8_t frame_interval,
                       bool scroll_left)
{
    oled_send_command(scroll_left ? 0x27 : 0x26); // 0x26 = right, 0x27 = left
    oled_send_command(0x00); // dummy byte
    oled_send_command(start_page); // start page address
    oled_send_command(frame_interval); // frame frequency (see datasheet table)
    oled_send_command(end_page); // end page address
    oled_send_command(0x00); // dummy byte
    oled_send_command(0xFF); // dummy byte
    oled_send_command(0x2F); // activate scroll (send last)
}

void oled_scroll_stop(void)
{
    oled_send_command(0x2E); // deactivate scroll

    // Datasheet recommends rewriting RAM content after stopping,
    // since the display pointer can be left in a scrolled state.
    oled_flush_fromtask();
}

void oled_invert(bool inverted)
{
    oled_send_command(inverted ? 0xA7 : 0xA6);
}
