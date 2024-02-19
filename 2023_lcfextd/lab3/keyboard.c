#include "../common/lib.h"
#include "kbc.h"

static bool ibf_full() {
  uint8_t st;
  util_sys_inb(KBC_STAT_REG, &st);
  return (st & KBC_STAT_IBF);
}

void enable_kbd_interrupts() {
  while (ibf_full()) {
    sleep_abit();
  }

  // send "read cmd byte"
  sys_outb(KBC_IN_BUF_CMD, KBC_CMD_READ_BYTE);
  sleep_abit();

  // read cmd byte
  uint8_t cmd_byte;
  util_sys_inb(KBC_OUT_BUF, &cmd_byte);

  cmd_byte |= KBC_CMD_BYTE_INT;

  while (ibf_full()) {
    sleep_abit();
  }

  // send "write cmd byte"
  sys_outb(KBC_IN_BUF_CMD, KBC_CMD_WRITE_BYTE);

  while (ibf_full()) {
    sleep_abit();
  }

  sys_outb(KBC_IN_BUF_ARG, cmd_byte);
}