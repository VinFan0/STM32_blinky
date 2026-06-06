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

void io_enable_clock(io_port port);
void io_set_mode(io_port port, uint8_t pin, io_mode mode);
void io_set_otype(io_port port, uint8_t, io_otype otype);

#endif // __IO_H_
