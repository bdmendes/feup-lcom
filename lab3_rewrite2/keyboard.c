#include "kbc.h"
#include <lcom/lcf.h>

static int kbc_hook_id = KBD_IRQ;
static uint8_t out_buf[2] = {0, 0};
static uint8_t byte_index = 0;

uint8_t *kbc_get_out_buf() { return out_buf; }

int kbc_subscribe_int(uint8_t *bit_no) {
  *bit_no = KBD_IRQ;
  if (sys_irqsetpolicy(KBD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &kbc_hook_id) !=
      OK) {
    printf("sys_irqsetpolicy failed");
    return !OK;
  }
  return OK;
}

int kbc_unsubscibe_int() {
  if (sys_irqrmpolicy(&kbc_hook_id) != OK) {
    printf("sys_irqrmpolicy failed");
    return !OK;
  }
  return OK;
}

int kbc_read_byte() {
  uint8_t status;
  if (util_sys_inb(KBD_STAT_REG, &status) != OK) {
    printf("sys_inb failed");
    return !OK;
  }

  int status_error = status & (KBD_STAT_PARITY | KBD_STAT_TIMEOUT);
  if (status_error) {
    perror("Parity or timeout error");
    return !OK;
  }

  int is_mouse_data = status & KBD_STAT_AUX;
  if (is_mouse_data) {
    return !OK;
  }

  int is_out_buf_full = status & KBD_STAT_OBF;
  if (!is_out_buf_full) {
    return !OK;
  }

  uint8_t *ptr = (uint8_t *)&out_buf + byte_index;
  if (util_sys_inb(KBD_OUT_BUF, ptr) != OK) {
    printf("sys_inb failed");
    return !OK;
  }

  if (out_buf[byte_index] == TWO_BYTE_CODE) {
    byte_index++;
    return !OK;
  } else {
    byte_index = 0;
  }

  return OK;
}

void kbc_interrupt_handler() { kbc_read_byte(); }
