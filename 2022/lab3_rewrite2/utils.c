#include <lcom/lcf.h>
#include <minix/sysutil.h>
#include <stdint.h>

int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
  uint16_t right = 0x00FF;
  *lsb = right & val;
  return OK;
}

int(util_get_MSB)(uint16_t val, uint8_t *msb) {
  uint16_t left = 0xFF00;
  *msb = (left & val) >> 8;
  return OK;
}

int(util_sys_inb)(int port, uint8_t *value) {
  uint32_t buf = 0;
  int ret = sys_inb(port, &buf);
  *value = (uint8_t)buf;
  return ret;
}
