#ifndef __SPI1_H_
#define __SPI1_H_

#include <stdint.h>

/*
 * SPI1 Pins on Nucleo-L476RG board
 * CS 	-> PB6
 * MOSI -> PA7
 * SCK 	-> PA5
 * DC 	-> PA9 (GPIO)
 * RES 	-> PA8 (GPIO)
 */
#define SPI1_PORT IO_PORT_A
#define SPI1_CS_PORT IO_PORT_B
#define SPI1_CS 6 // PB6
#define SPI1_MOSI 7
#define SPI1_SCK 5
#define SPI1_DC 9
#define SPI1_RES 8

/*
 * Common SPI functions
 */
#define SPI1_CS_LOW() GPIOB->ODR &= ~(1U << SPI1_CS)
#define SPI1_CS_HIGH() GPIOB->ODR |= (1U << SPI1_CS)

#define SPI1_DC_CMD() GPIOA->ODR &= ~(1U << SPI1_DC)
#define SPI1_DC_DAT() GPIOA->ODR |= (1U << SPI1_DC)

#define SPI1_RES_LOW() GPIOA->ODR &= ~(1U << SPI1_RES)
#define SPI1_RES_HIGH() GPIOA->ODR |= (1U << SPI1_RES)

void spi1_init(void);
void spi1_send(uint8_t data);
void spi1_debug(void);

#endif // __SPI1_H_
