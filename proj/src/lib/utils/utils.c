#include "utils.h"
#include <minix/sysutil.h>

int (util_get_LSB)(uint16_t val, uint8_t *lsb) {
  uint16_t right = 0x00FF;
  *lsb = right & val;
  return 1;
}

int (util_get_MSB)(uint16_t val, uint8_t *msb) {
  uint16_t left = 0xFF00;
  *msb = (left & val) >> 8;
  return 1;
}

int (util_sys_inb)(int port, uint8_t *value) {
  uint32_t long_value = 0;

  if (sys_inb(port,&long_value) != OK){
    printf("error reading port 0x%x\n",port);
    return !OK;
  }

  *value = (uint8_t) long_value;
  return OK;
}

void (delay_milli_seconds)(unsigned milli_seconds){
  tickdelay(micros_to_ticks(milli_seconds*1000));
}

void (delay_micro_seconds)(unsigned micro_seconds){
  tickdelay(micros_to_ticks(micro_seconds));
}

void (delay_seconds)(unsigned seconds) {
  tickdelay(micros_to_ticks(seconds*1000000));
}

uint32_t (bit_range)(uint8_t start, uint8_t end){
  uint32_t res = 0;
  for (uint8_t i = start; i < end; ++i){
    res |= BIT(i);
  }
  return res;
}

uint8_t (get_byte)(uint32_t value, uint8_t byte){
  return (uint8_t) (value >> (byte*8));
}

uint8_t (convert_bcd_to_binary)(uint8_t value) {
  return (value >> 4) * 10 + (value & 0x0F);
}

uint8_t (convert_binary_to_bcd)(uint8_t value) {
  return ((value/10) << 4) | (value%10);
}
