#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

int timer_counter = 0;
static int timer_hook_id = 0;

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {

  uint8_t st = 0;
  timer_get_conf(timer, &st);
  st = (0xF & st) | TIMER_LSB_MSB | (timer << 6);
  sys_outb(TIMER_CTRL, st);

  uint8_t read_back_command = TIMER_RB_SEL(timer) | TIMER_RB_STATUS_ | TIMER_RB_CMD;
  sys_outb(TIMER_CTRL, read_back_command);
  uint16_t ticks = TIMER_FREQ / freq;

  uint8_t less_sig_ticks; util_get_LSB(ticks, &less_sig_ticks);
  uint8_t most_sig_ticks; util_get_MSB(ticks, &most_sig_ticks);

  return sys_outb(TIMER_0 + timer, less_sig_ticks) &
         sys_outb(TIMER_0 + timer, most_sig_ticks);
}

int (timer_subscribe_int)(uint8_t *bit_no) {
  *bit_no = timer_hook_id;
  return sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &timer_hook_id);
}

int (timer_unsubscribe_int)() {
  return sys_irqrmpolicy(&timer_hook_id);
}

void (timer_int_handler)() {
  timer_counter ++;
}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {
  uint8_t read_back_command = TIMER_RB_SEL(timer) | TIMER_RB_COUNT_ | TIMER_RB_CMD;
  sys_outb(TIMER_CTRL, read_back_command);
  return util_sys_inb(TIMER_0 + timer, st);
}

int (timer_display_conf)(uint8_t timer, uint8_t st,
                        enum timer_status_field field) {
  union timer_status_field_val field_value;
  switch (field) {
    case tsf_all:
      field_value.byte = st;
      break;
    case tsf_initial:
      field_value.in_mode = (TIMER_LSB_MSB & st) >> 4;
      break;
    case tsf_mode:
      field_value.count_mode = ((BIT(3) | BIT(2) | BIT(1)) & st) >> 1;
      if (BIT(1) & field_value.count_mode) {
        field_value.count_mode &= 0x3;  // make dont care bytes 0 for future compatibility
      }
      break;
    case tsf_base:
      field_value.bcd = (BIT(0) & st);
      break;
  }
  return timer_print_config(timer, field, field_value);;
}
