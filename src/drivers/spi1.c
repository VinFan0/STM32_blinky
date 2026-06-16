#include "stm32l476xx.h"
#include "drivers/spi1.h"
#include "drivers/io.h"
#include "drivers/uart.h"

void spi1_init(void)
{
    // Enable Clocks for GPIOB and SPI1
    io_enable_clock(SPI1_PORT);
    io_enable_clock(SPI1_CS_PORT);
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
    volatile uint32_t dummy = RCC->APB2ENR;
    (void)dummy;

    // Set SCK and MOSI alternate functions
    io_set_afr(SPI1_PORT, SPI1_SCK, IO_AF5);
    io_set_mode(SPI1_PORT, SPI1_SCK, IO_MODE_AF);

    io_set_afr(SPI1_PORT, SPI1_MOSI, IO_AF5);
    io_set_mode(SPI1_PORT, SPI1_MOSI, IO_MODE_AF);

    // Set CS, RES, DC GPIO outputs
    io_set_mode(SPI1_CS_PORT, SPI1_CS, IO_MODE_OUTPUT);
    io_set_mode(SPI1_PORT, SPI1_RES, IO_MODE_OUTPUT);
    io_set_mode(SPI1_PORT, SPI1_DC, IO_MODE_OUTPUT);

    io_set_otype(SPI1_CS_PORT, SPI1_CS, IO_OTYPE_PP);
    io_set_otype(SPI1_PORT, SPI1_RES, IO_OTYPE_PP);
    io_set_otype(SPI1_PORT, SPI1_DC, IO_OTYPE_PP);

    io_set_ospeed(SPI1_CS_PORT, SPI1_CS, IO_OSPEED_HIGH);
    io_set_ospeed(SPI1_PORT, SPI1_RES, IO_OSPEED_HIGH);
    io_set_ospeed(SPI1_PORT, SPI1_DC, IO_OSPEED_HIGH);

    io_set_pupdr(SPI1_CS_PORT, SPI1_CS, IO_PUPD_NONE);
    io_set_pupdr(SPI1_PORT, SPI1_RES, IO_PUPD_NONE);
    io_set_pupdr(SPI1_PORT, SPI1_DC, IO_PUPD_NONE);

    // SPI1 config
    SPI1->CR2 = (0x7 << SPI_CR2_DS_Pos); // 8 bit data length

    // clang-format off
    SPI1->CR1 = SPI_CR1_MSTR			// Master Configuration
	    	| SPI_CR1_SSM			// Software Slave Management
		| SPI_CR1_SSI 			// Internal Slave Select
		| (0x2 << SPI_CR1_BR_Pos) 	// BR: f_PCLK/256
		| SPI_CR1_CPOL			// Clock polarity 1: CK to 1 when idle
	        | SPI_CR1_CPHA			// Clock phase 1: second clock transition is first date capture edge
		| SPI_CR1_SPE;
    // clang-format on
}

void spi1_debug(void)
{
    uint32_t rcc_apb2 = RCC->APB2ENR;
    uint32_t cr1 = SPI1->CR1;
    uint32_t cr2 = SPI1->CR2;
    uint32_t sr = SPI1->SR;
    uint32_t moder_a = GPIOA->MODER;
    uint32_t afrl_a = GPIOA->AFR[0];

    for (int i = 0; i < 10; i++)
        uart_send_char('*');
    uart_send_char('\r');
    uart_send_char('\n');

    uart_print_hex("RCC_APB2ENR", rcc_apb2);
    uart_print_hex("SPI1_CR1    ", cr1);
    uart_print_hex("SPI1_CR1    ", cr2);
    uart_print_hex("SPI1_SR     ", sr);
    uart_print_hex("GPIOA_MODER ", moder_a);
    uart_print_hex("GPIOA_AFRL  ", afrl_a);
}

void spi1_send(uint8_t data)
{
    // clang-format off
        while (!(SPI1->SR & SPI_SR_TXE));		// While transmit buffer not empty
        *((volatile uint8_t *)&SPI1->DR) = data;	// Write data to data register
        while (SPI1->SR & SPI_SR_BSY);		// Wait for busy flag reset (TX finished)
        // clang-format on    
}
