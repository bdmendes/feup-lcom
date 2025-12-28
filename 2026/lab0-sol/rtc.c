#include "rtc.h"
#include "bitwise.h"

#include <minix/syslib.h>
#include <minix/sysutil.h>

static int rtc_is_bcd() {
  sys_outb(0x70, 0xB);
  uint32_t mode;
  sys_inb(0x71, &mode);
  return !is_set(mode, 2);
}

static int bcd_to_binary(uint8_t bcd) {
  return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

static int rtc_read_reg(uint8_t reg, uint8_t *buf) {
  uint8_t tries = 3;
  while (tries--) {
    sys_outb(0x70, 0xA);
    uint32_t reg_a;
    sys_inb(0x71, &reg_a);
    if (reg_a & mask(7, MSK_END)) {
      tickdelay(micros_to_ticks(20000));
    } else {
      sys_outb(0x70, reg);
      uint32_t value;
      sys_inb(0x71, &value);
      *buf = (uint8_t)value;
      return 0;
    }
  }
  return -1;
}

int rtc_read_date(rtc_date *date) {
  bool is_bcd = rtc_is_bcd();

  rtc_read_reg(0x07, &date->day);
  if (is_bcd) {
    date->day = bcd_to_binary(date->day);
  }

  rtc_read_reg(0x08, &date->month);
  if (is_bcd) {
    date->month = bcd_to_binary(date->month);
  }

  rtc_read_reg(0x09, &date->year);
  if (is_bcd) {
    date->year = bcd_to_binary(date->year);
  }

  return 0;
}
