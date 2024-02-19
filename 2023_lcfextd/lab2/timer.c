#include "../common/lib.h"
#include "i8254.h"

int(timer_get_conf)(uint8_t timer, uint8_t *st) {
  uint8_t readback_cmd = TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer);
  sys_outb(TIMER_CTRL, readback_cmd);

  return util_sys_inb(TIMER_0, st);
}

int(timer_display_conf)(uint8_t timer, uint8_t conf,
                        enum timer_status_field field) {
  union timer_status_field_val status;
  switch (field) {
  case tsf_all:
    status.byte = conf;
    break;
  case tsf_base:
    status.bcd = conf & BIT(0);
    break;
  case tsf_initial:
    status.in_mode = (conf >> 4) & (BIT(0) | BIT(1));
    break;
  case tsf_mode:
    conf = (conf >> 1) & (BIT(0) | BIT(1) | BIT(2));
    if (conf > 5) {
      conf = conf & (BIT(0) | BIT(1));
    }
    status.count_mode = conf;
  }

  return timer_print_config(timer, field, status);
}

int(timer_set_frequency)(uint8_t timer, uint32_t freq) {
  uint8_t st;
  timer_get_conf(timer, &st);

  // mask different parameters
  st &= BIT(0) | BIT(1) | BIT(2) | BIT(3);

  // set init mode to LSB followed by MSB
  st |= BIT(4) | BIT(5);

  // select timer
  st |= (timer << 6);

  // write config to control
  sys_outb(TIMER_CTRL, st);

  // write counter to timer
  uint32_t counter = TIMER_FREQ / freq;
  uint8_t lsb = counter;
  uint8_t msb = (counter >> 8);
  sys_outb(TIMER_0 + timer, lsb);
  sys_outb(TIMER_0 + timer, msb);

  return 0;
}