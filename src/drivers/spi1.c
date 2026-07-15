#include "stm32l476xx.h"
#include "drivers/spi1.h"
#include "drivers/io.h"
#include "drivers/uart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "stream_buffer.h"
#include "semphr.h"

/*
 * +1 so a full 512-byte oled_buf flush fits in one shot without the
 * sending task ever having to block waiting for the ISR to drain it.
 * (A stream buffer of size N only holds N-1 usable bytes.)
 */
#define SPI1_TX_STREAM_SIZE 513
#define SPI1_TX_TRIGGER_LEVEL 1

static StreamBufferHandle_t spi1TxStream;
static SemaphoreHandle_t spi1TxDoneSem; // given by ISR once stream is drained
static SemaphoreHandle_t spi1TxMutex; // serializes concurrent callers
static volatile size_t spi1TxRemaining;

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

    spi1TxStream = xStreamBufferCreate(SPI1_TX_STREAM_SIZE, SPI1_TX_TRIGGER_LEVEL);
    spi1TxDoneSem = xSemaphoreCreateBinary();
    spi1TxMutex = xSemaphoreCreateMutex();

    /*
     * Priority must be numerically >= configMAX_SYSCALL_INTERRUPT_PRIORITY
     * (i.e. logically "at or below" that threshold) since the handler calls
     * FreeRTOS ...FromISR() APIs.
     */
    NVIC_SetPriority(SPI1_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
    NVIC_EnableIRQ(SPI1_IRQn);
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
        uart_send_char_polling('*');
    uart_send_char_polling('\r');
    uart_send_char_polling('\n');

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

/*
 * Streams `len` bytes out over SPI1 via the TXE interrupt, without the
 * calling task busy-polling the peripheral for the whole transfer.
 * Blocks (sleeps) the calling task until every byte has been clocked out.
 * Caller is still responsible for CS/DC around this call.
 */
void spi1_send_stream(const uint8_t *data, size_t len)
{
    if (len == 0)
        return;

    xSemaphoreTake(spi1TxMutex, portMAX_DELAY);

    spi1TxRemaining = len;

    size_t sent = 0;
    while (sent < len) {
        sent += xStreamBufferSend(spi1TxStream, data + sent, len - sent, portMAX_DELAY);
    }

    // Kick off transmission
    SPI1->CR2 |= SPI_CR2_TXEIE;

    // Sleep until the ISR has pulled the last byte out of the stream buffer
    xSemaphoreTake(spi1TxDoneSem, portMAX_DELAY);

    // The last byte is still shifting out of the SPI shift register at this
    // point (TXE fires when DR is empty, not when the byte is fully sent).
    // This is a single-byte-length wait, not a whole-buffer one.
    while (SPI1->SR & SPI_SR_BSY);

    xSemaphoreGive(spi1TxMutex);
}

void SPI1_IRQHandler(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (SPI1->SR & SPI_SR_TXE) {
        uint8_t byte;
        size_t received = xStreamBufferReceiveFromISR(
            spi1TxStream, &byte, 1, &xHigherPriorityTaskWoken);

        if (received == 1) {
            *((volatile uint8_t *)&SPI1->DR) = byte;
            spi1TxRemaining--;
            if (spi1TxRemaining == 0) {
                SPI1->CR2 &= ~SPI_CR2_TXEIE;
                xSemaphoreGiveFromISR(spi1TxDoneSem, &xHigherPriorityTaskWoken);
            }
        } else {
            // Nothing queued right now — stop interrupting until more is sent.
            SPI1->CR2 &= ~SPI_CR2_TXEIE;
        }
    }

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

