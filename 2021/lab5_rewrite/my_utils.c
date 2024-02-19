#include "my_utils.h"

void (delay_milli_seconds)(unsigned milli_seconds){
  tickdelay(micros_to_ticks(milli_seconds*1000));
}

void (delay_micro_seconds)(unsigned micro_seconds){
  tickdelay(micros_to_ticks(micro_seconds));
}

void (delay_seconds)(unsigned seconds) {
  tickdelay(micros_to_ticks(seconds*1000000));
}

uint32_t bit_range(uint8_t start, uint8_t end){
  uint32_t res = 0;
  for (uint8_t i = start; i < end; ++i){
    res |= BIT(i);
  }
  return res;
}

int (util_sys_inb)(int port, uint8_t *value) {
  
  uint32_t long_value = 0;
  int success = sys_inb(port,&long_value);
  *value = (uint8_t) long_value;
  
  return success;
}
