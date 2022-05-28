#include "uart.h"

#define COM1_IRQ 4
#define COM2_IRQ 3

/* LCR */
#define LCR_DLAB BIT(7)

int read_uart_config(struct uart_config_t *uart_config) {
  uint8_t config = 0;
  if (util_sys_inb(COM1 + 3, &config) != OK) {
    printf("error reading serial port config");
    return !OK;
  }

  /* Extract config */
  uint8_t bits_per_char_msk = config & (BIT(0) | BIT(1));
  uart_config->no_bits_per_char = bits_per_char_msk + 5;

  uart_config->no_stop_bits = ((config >> 2) & BIT(0)) + 1;

  uint8_t parity_msk = (config >> 3) & (BIT(0) | BIT(1) | BIT(2));
  switch (parity_msk) {
  case 0x1:
    uart_config->parity = ODD_PARITY;
    break;
  case 0x3:
    uart_config->parity = EVEN_PARITY;
    break;
  case 0x5:
    uart_config->parity = PARITY_ALWAYS_1;
    break;
  case 0x7:
    uart_config->parity = PARITY_ALWAYS_0;
    break;
  }

  /* Request access to the bitrate */
  config |= LCR_DLAB;
  if (sys_outb(COM1 + 3, config) != OK) {
    printf("error writing serial port config");
    return !OK;
  }

  /* Extract bit rate */
  uint8_t msb_bitrate = 0;
  uint8_t lsb_bitrate = 0;
  if (util_sys_inb(COM1 + 0, &lsb_bitrate) != OK) {
    printf("error reading serial port lsb bitrate");
    return !OK;
  }
  if (util_sys_inb(COM1 + 1, &msb_bitrate) != OK) {
    printf("error reading serial port msb bitrate");
    return !OK;
  }
  uart_config->bitrate =
      115200 / ((((uint16_t)msb_bitrate) << 8) | lsb_bitrate);

  /* Restore registers 0 and 1 access mode */
  config &= ~LCR_DLAB;
  if (sys_outb(COM1 + 3, config) != OK) {
    printf("error writing serial port config");
    return !OK;
  }

  return OK;
}
