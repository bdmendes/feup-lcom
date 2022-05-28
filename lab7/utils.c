#include <lcom/lcf.h>
#include <minix/sysutil.h>
#include <stdint.h>

#ifdef LAB3
extern int count;
#endif

int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
  uint16_t right = 0x00FF;
  *lsb = right & val;
  return 1;
}

int(util_get_MSB)(uint16_t val, uint8_t *msb) {
  uint16_t left = 0xFF00;
  *msb = (left & val) >> 8;
  return 1;
}

int(util_sys_inb)(int port, uint8_t *value) {
  uint32_t long_value = 0;
  int success = sys_inb(port, &long_value);
  *value = (uint8_t)long_value;

#ifdef LAB3
  count++;
#endif

  return success;
}
