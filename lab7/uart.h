#ifndef UART_H
#define UART_H

#include <lcom/lcf.h>

#define COM1 0x3F8
#define COM2 0x2F8

enum parity_t {
  NO_PARITY,
  ODD_PARITY,
  EVEN_PARITY,
  PARITY_ALWAYS_1,
  PARITY_ALWAYS_0
};

struct uart_config_t {
  uint64_t bitrate;
  uint8_t no_bits_per_char;
  uint8_t no_stop_bits;
  enum parity_t parity;
};

int read_uart_config(struct uart_config_t *uart_config);

#endif
