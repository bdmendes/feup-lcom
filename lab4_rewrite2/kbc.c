#include "kbc.h"
#include <lcom/lcf.h>

#define MAX_TRIES 3
#define delay() tickdelay(micros_to_ticks(20000))

int kbc_write_cmd(uint8_t cmd) {
  uint8_t status;
  int tries = 0;

  while (tries++ < MAX_TRIES) {
    if (util_sys_inb(KBC_STAT_REG, &status) != OK) {
      printf("Error reading status register");
      return 1;
    }

    if (!(status & KBC_STAT_IBF)) {
      break;
    }
    delay();
  }

  if (tries == MAX_TRIES) {
    printf("Error writing command");
    return 1;
  }

  if (sys_outb(KBC_STAT_REG, cmd) != OK) {
    printf("Error writing command");
    return 1;
  }

  return 0;
}

int kbc_read_obf(uint8_t *data) {
  uint8_t status;
  int tries = 0;

  while (tries++ < MAX_TRIES) {
    if (util_sys_inb(KBC_STAT_REG, &status) != OK) {
      printf("Error reading status register");
      return 1;
    }

    if (status & KBC_STAT_OBF) {
      break;
    }
    delay();
  }

  if (tries == MAX_TRIES) {
    printf("Error reading output buffer");
    return 1;
  }

  if (util_sys_inb(KBC_OUT_BUF, data) != OK) {
    printf("Error reading output buffer");
    return 1;
  }

  return 0;
}

int kbc_read_status(uint8_t *data) {
  if (util_sys_inb(KBC_STAT_REG, data) != OK) {
    printf("Error reading status register");
    return 1;
  }

  return 0;
}