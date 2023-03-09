#include "utils.h"

int(util_sys_inb)(int port, uint8_t *value) {
  uint32_t extended_value = 0;
  if (sys_inb(port, &extended_value) != OK) {
    printf("Error on util_sys_inb\n");
    return !OK;
  }
  *value = (uint8_t) (0x00FF & extended_value);
  return OK;
}

void (delay_milli_seconds)(unsigned milli_seconds){
  tickdelay(micros_to_ticks(milli_seconds*1000));
}

void (delay_micro_seconds)(unsigned micro_seconds){
  tickdelay(micros_to_ticks(micro_seconds));
}
