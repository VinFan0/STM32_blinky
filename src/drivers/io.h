#ifndef __IO_H_
#define __IO_H_

#include <stdint.h>

typedef enum {
    IO_PORT_A,
    IO_PORT_B,
    IO_PORT_C,
    IO_PORT_D,
    IO_PORT_E,
    IO_PORT_F,
    IO_PORT_G,
    IO_PORT_H
} io_port;

typedef enum {
    IO_MODE_INPUT,
    IO_MODE_OUTPUT,
    IO_MODE_AF,
    IO_MODE_ANALOG
} io_mode;

typedef enum {
    IO_OTYPE_PP, // Output Push-Pull
    IO_OTYPE_OD // Output Open-Drain
} io_otype;

typedef enum {
    IO_OSPEED_LOW,
    IO_OSPEED_MED,
    IO_OSPEED_HIGH,
    IO_OSPEED_VHIGH
} io_ospeed;

typedef enum {
    IO_PUPDR_NONE, // bits 00
    IO_PUPDR_PU, // bits 01
    IO_PUPDR_PD // bits 10
    // bits 11: Reserved
} io_pupd;

void io_enable_clock(io_port port);
void io_set_mode(io_port port, uint8_t pin, io_mode mode);
void io_set_otype(io_port port, uint8_t pin, io_otype otype);
void io_set_ospeed(io_port port, uint8_t pin, io_ospeed ospeed);
void io_set_pupdr(io_port port, uint8_t pin, io_pupd pupd);

#endif // __IO_H_
