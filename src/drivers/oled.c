#include <string.h>
#include "stm32l476xx.h"
#include "drivers/oled.h"
#include "io.h"
#include "common/utils.h"
#include "drivers/spi1.h"

uint8_t oled_buf[512];

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
    oled_send_command(0x8F);
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
    for (uint16_t i = 0; i < sizeof(oled_buf); i++) {
        spi1_send(oled_buf[i]);
    }
    SPI1_CS_HIGH();
}

void oled_set_pixel(uint8_t x, uint8_t y, bool on)
{
    // Each byte covers 8 vertial pixels (one "page")
    uint16_t idx = x + ((y >> 3) << 7);
    if (on) {
        oled_buf[idx] |= (1U << (y & 0x7)); // y % 8
    } else {
        oled_buf[idx] &= ~(1U << (y & 0x7));
    }
}
