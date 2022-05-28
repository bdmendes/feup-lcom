#include "uart.h"

#define COM1_IRQ 4
#define COM1 0x3F8

/* LCR */
#define LCR_DLAB BIT(7)

int hook_id = COM1_IRQ;

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

static int enable_uart_fifo_capabilities() {
  uint8_t fifo_config = 0;
  if (util_sys_inb(COM1 + 2, &fifo_config) != OK) {
    printf("error reading serial port fifo_config");
    return !OK;
  }

  // enable fifo
  fifo_config |= BIT(0);
  // 8 ready interrupts
  fifo_config |= BIT(7);
  fifo_config &= ~BIT(6);

  if (sys_outb(COM1 + 2, fifo_config) != OK) {
    printf("error writing serial port fifo config");
    return !OK;
  }
  return OK;
}

static int reset_uart_fifo_data() {
  uint8_t fifo_config = 0;
  if (util_sys_inb(COM1 + 2, &fifo_config) != OK) {
    printf("error reading serial port fifo_config");
    return !OK;
  }

  // reset fifo data
  fifo_config |= BIT(1) | BIT(2);

  if (sys_outb(COM1 + 2, fifo_config) != OK) {
    printf("error writing serial port fifo config");
    return !OK;
  }
  return OK;
}

void uart_ih() {
  /* Handles received data only */

  uint8_t lsr = 0;
  if (util_sys_inb(COM1 + 5, &lsr) != OK) {
    printf("could not read uart lsr");
    return;
  }

  bool has_errored = false;
  if (lsr & (BIT(1) | BIT(2) | BIT(3))) {
    has_errored = true;
  }

  uint8_t iir = 0;
  if (util_sys_inb(COM1 + 2, &iir) != OK) {
    printf("could not read uart iir");
    return;
  }
  if (!(iir & BIT(2))) {
    return;
  } else {
    uint8_t data;
    if (util_sys_inb(COM1, &data) != OK) {
      printf("error reading data");
    } else {
      if (has_errored) {
        printf("comm error...");
      } else {
        printf("received char: %u\n", data);
        fflush(stdout);
        // send to application level handler, a queue or state machine
      }
    }
  }
}

static int enable_receive_data_interrupts() {
  uint8_t ier = 0;
  if (util_sys_inb(COM1 + 1, &ier) != OK) {
    printf("error reading serial port ier");
    return !OK;
  }

  ier |= BIT(0);
  ier &= ~BIT(1);
  ier &= ~BIT(2);
  ier &= ~BIT(3);

  if (sys_outb(COM1 + 1, ier) != OK) {
    printf("error writing serial port fifo config");
    return !OK;
  }
  return OK;
}

int uart_subscribe_int(int *bit_no) {
  *bit_no = hook_id;
  enable_uart_fifo_capabilities();
  reset_uart_fifo_data();
  enable_receive_data_interrupts();
  if (sys_irqsetpolicy(COM1_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id) !=
      OK) {
    printf("error subscribing uart interrupts");
    return !OK;
  }
  return OK;
}

int uart_unsubscribe_int() {
  if (sys_irqrmpolicy(&hook_id) != OK) {
    printf("could not unsubscribe uart interrupts");
    return !OK;
  }
  return OK;
}
