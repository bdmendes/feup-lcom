#include "../common/lib.h"
#include "kbc.h"

static void wait_until_ib_empty() {
  for (;;) {
    uint8_t st;
    util_sys_inb(KBC_STAT_REG, &st);
    if (!(st & KBC_STAT_IBF)) {
      return;
    }
    sleep_abit();
  }
}

int mouse_send_cmd(uint8_t arg) {
  uint8_t ans;

  wait_until_ib_empty();
  sys_outb(KBC_IN_BUF_CMD, KBC_CMD_WRITE_MOUSE_BYTE);
  sleep_abit();

ISSUE_ARG:
  wait_until_ib_empty();
  sys_outb(KBC_IN_BUF_ARG, arg);
  sleep_abit();

  util_sys_inb(KBC_OUT_BUF, &ans);
  if (ans != MOUSE_RESPONSE_ACK) {
    goto ISSUE_ARG;
  }

  return 0;
}