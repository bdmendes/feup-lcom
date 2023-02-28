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
    perror("Mouse data");
    return !OK;
  }

  int obf_full = status & KBD_STAT_OBF;
  if (!obf_full) {
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

int kbc_write_cmd_byte(uint8_t cmd_byte) {
  for (int tries = 0; tries < 3; tries++) {
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

    int ibf_full = status & KBD_STAT_IBF;
    if (ibf_full) {
      micro_delay(20000);
      continue;
    }
    break;
  }

  if (sys_outb(KBD_STAT_REG, KBD_CMD_WRITE_BYTE) != OK) {
    printf("sys_outb failed");
    return !OK;
  }

  if (sys_outb(KBD_IN_BUF, cmd_byte) != OK) {
    printf("sys_outb failed");
    return !OK;
  }

  return OK;
}

int kbc_read_cmd_byte(uint8_t *cmd_byte) {
  if (kbc_write_cmd_byte(KBD_CMD_READ_BYTE) != OK) {
    printf("kbc_write_cmd_byte failed");
    return !OK;
  }

  if (util_sys_inb(KBD_OUT_BUF, cmd_byte) != OK) {
    printf("sys_inb failed");
    return !OK;
  }

  return OK;
}

int kbc_disable_int() {
  uint8_t cmd_byte;
  if (kbc_read_cmd_byte(&cmd_byte) != OK) {
    printf("kbc_read_cmd_byte failed");
    return !OK;
  }

  cmd_byte &= ~KBD_CB_INT;

  if (kbc_write_cmd_byte(cmd_byte) != OK) {
    printf("kbc_write_cmd_byte failed");
    return !OK;
  }

  return OK;
}

int kbc_enable_int() {
  uint8_t cmd_byte;
  if (kbc_read_cmd_byte(&cmd_byte) != OK) {
    printf("kbc_read_cmd_byte failed");
    return !OK;
  }

  cmd_byte |= KBD_CB_INT;

  if (kbc_write_cmd_byte(cmd_byte) != OK) {
    printf("kbc_write_cmd_byte failed");
    return !OK;
  }

  return OK;
}

void kbc_interrupt_handler() { kbc_read_byte(); }
