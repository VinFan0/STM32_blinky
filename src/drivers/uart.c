#include <stm32l476xx.h>
#include <string.h>
#include "drivers/uart.h"
#include "drivers/io.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "stream_buffer.h"

/* Nucleo-L476RG ST-Link virtual COM connected to USART2
 * TX -> PA2
 * RX -> PA3
 * Both AF7
 */

static char rx_buf[RX_BUF_SIZE];
static uint32_t rx_head = 0;
static uint32_t rx_tail = 0;

// **************************************************
// RTOS Tasks and variables
// **************************************************
static StreamBufferHandle_t uartTxStream;
static SemaphoreHandle_t uartRxSem;
// static TaskHandle_t uartRxTaskHandle;
static TaskHandle_t uartTxTaskHandle;

static void uart_tx_task(void *arg)
{
    (void)arg;
    for (;;) {
        uart_transmit("UART tx from RTOS task!\r\n");
        vTaskDelay(1000);
    }
}

static void uart_rx_task(void *arg)
{
    (void)arg;
    for (;;) {
        xSemaphoreTake(uartRxSem, portMAX_DELAY);
        while (uart_data_available()) {
            char c = uart_read_char();
            uart_send_char_polling(c);
        }
    }
}

void uart_start_tasks(void)
{
    (void)uart_rx_task; // xTaskCreate(uart_rx_task, "uart_rx", 256, NULL, 2, &uartRxTaskHandle);
    xTaskCreate(uart_tx_task, "uart_tx", 256, NULL, 2, &uartTxTaskHandle);
}

/* USART2 Init Workflow
 * Enable port A and USART2 clocks
 * Configure TX and RX pins
 * - Alternate Function Mode
 * - AF7
 * - Set TX output speed high
 * - Disable PUPDR
 * Configure USART2
 * - Disable USART pre-config
 * - Set buad rate: PCLK1 defaults to 4MHz post-reset
 *   - BRR = PCLK1 / baud (4MHz / 115200 = ~35)
 * - USART Defaults (8 data bits, no parity, 1 stop bit)
 * - Enable TX and RS, then enable USART2
 * - Wait until USART is ready to transmit
 */
void uart_init(void)
{
    // Configure TX and RX pins
    io_enable_clock(IO_PORT_A); // Enable USART clock
    RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN; // Enable USART2

    io_set_mode(IO_PORT_A, 2, IO_MODE_AF); // Set PA2-3 alternate function
    io_set_mode(IO_PORT_A, 3, IO_MODE_AF);

    io_set_afr(IO_PORT_A, 2, IO_AF7); // Set PA2-3 AF7 (USART1/2/3)
    io_set_afr(IO_PORT_A, 3, IO_AF7);

    io_set_ospeed(IO_PORT_A, 2, IO_OSPEED_HIGH); // High speed output

    io_set_pupdr(IO_PORT_A, 2, IO_PUPD_NONE); // No pull-up/down
    io_set_pupdr(IO_PORT_A, 3, IO_PUPD_NONE);

    // Configure USART2
    USART2->CR1 &= ~USART_CR1_UE; // Disable USART

    USART2->BRR = 35; // BRR = PCLK1 / baud (4MHz / 115200 = ~35) // Set Baud rate 115200

    USART2->CR1 |= USART_CR1_TE // Enable transmitter,
        | USART_CR1_RE // Enable receiver
        | USART_CR1_UE // Enable USART
        | USART_CR1_RXNEIE; // Enable RXNE interrupt

    USART2->ISR |= USART_ICR_ORECF // Clear ORE flag in ISR
        | USART_ICR_FECF // Clear FE flag in ISR
        | USART_ICR_NCF // Clear NF flag in ISR
        | USART_ICR_PECF; // Clear PE flag in ISR

    uartRxSem = xSemaphoreCreateBinary(); // Create semaphore for UART2 RX
    uartTxStream = xStreamBufferCreate(128, 1); // 128-byte buffer, wake on 1 byte

    // Set priority to max allowed for syscall function
    NVIC_SetPriority(USART2_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
    NVIC_EnableIRQ(USART2_IRQn); // Enable USART2 global interrupt

    if (USART2->ISR & USART_ISR_RXNE) // If Read Data Register Not Empty
        (void)USART2->RDR;

    while (!(USART2->ISR & USART_ISR_TEACK)) { }; // Wait for Transmit Enable ACK
    while (!(USART2->ISR & USART_ISR_REACK)) { }; // Wait for Receive Enable ACK
}

void uart_send_char_polling(char c)
{
    while (!(USART2->ISR & USART_ISR_TXE)) {
    }; // Wait for Transmit Data Register to empty into shift register
    USART2->TDR = c; // Write character to Transmit Data Register
}

void uart_transmit(const char *str)
{
    size_t len = strlen(str);
    xStreamBufferSend(uartTxStream, str, len, portMAX_DELAY);
    USART2->CR1 |= USART_CR1_TXEIE; // Kick on TX interrupt to start draining

    /*
    while (*str) {
    uart_send_char_polling(*str++); // Send characters to TDR one at a time
    */
}

void uart_flush(void)
{
    while (!(USART2->ISR & USART_ISR_TC)) { }; // Wait for transmission complete
}

int uart_check_input_polling(char *out)
{
    if (!(USART2->ISR
          & USART_ISR_RXNE)) // Return if shift register is still writing to Recieve Data Register
        return 0;
    *out = (char)(USART2->RDR & 0xFF); // Write character to out variable and return affirmative
    return 1;
}

int uart_data_available(void)
{
    return rx_head != rx_tail;
}

char uart_read_char(void)
{
    char c = rx_buf[rx_tail];
    rx_tail = rx_next(rx_tail);
    return c;
}

void USART2_IRQHandler(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    // TX Ready Interrupt
    if (USART2->ISR & USART_ISR_TXE) { // If TDR empty
        uint8_t byte;
        if (xStreamBufferReceiveFromISR(
                uartTxStream, &byte, 1,
                &xHigherPriorityTaskWoken)) { // If byte ready in uartTxStream
            USART2->TDR = byte; // Put byte in TDR
        } else {
            USART2->CR1 &= ~USART_CR1_TXEIE; // Clear interrupt when nothing to send
        }
    }

    // RX Ready Interrupt
    if (USART2->ISR & USART_ISR_RXNE) { // If shift registers finished writing to RDR
        char c = (char)(USART2->RDR & 0xFF); // Get char from RDR

        uint32_t next = rx_next(rx_head); // Update ring buffer, if there's room
        if (next != rx_tail) {
            rx_buf[rx_head] = c;
            rx_head = next;
        }
        xSemaphoreGiveFromISR(uartRxSem, &xHigherPriorityTaskWoken); // Give up RX semaphore

        xStreamBufferSendFromISR(uartTxStream, &c, 1, &xHigherPriorityTaskWoken);
        xStreamBufferSendFromISR(uartTxStream, "\r\n", 2, &xHigherPriorityTaskWoken);
        USART2->CR1 |= USART_CR1_TXEIE;
    }

    // OverRun Error Interrupt
    if (USART2->ISR & USART_ISR_ORE) { // If OverRun error, clear OverRun error flag
        USART2->ICR |= USART_ICR_ORECF;
    }

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void uart_print_hex(const char *label, uint32_t val)
{
    char buf[12];
    uart_transmit(label);
    uart_transmit(": 0x");

    for (int i = 7; i >= 0; i--) {
        uint8_t nibble = (val >> (i << 2)) & 0xF;
        buf[7 - i] = nibble < 10 ? '0' + nibble : 'A' + nibble - 10;
    }
    buf[8] = '\r';
    buf[9] = '\n';
    buf[10] = '\0';
    uart_transmit(buf);
}
