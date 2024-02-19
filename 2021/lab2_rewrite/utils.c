#include <lcom/lcf.h>

#include <stdint.h>

int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
  *lsb = (uint8_t) (val & 0x00FF);
  return OK;
}

int(util_get_MSB)(uint16_t val, uint8_t *msb) {
  *msb = (uint8_t) ((val & 0xFF00) >> 8);
  return OK;
}

int (util_sys_inb)(int port, uint8_t *value) {
  uint32_t value_extended = (uint32_t) value;
  int success = sys_inb(port, &value_extended);
  *value = (uint8_t) value_extended;
  return success;
}
