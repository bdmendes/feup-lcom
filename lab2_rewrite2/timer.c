#include "i8254.h"
#include <lcom/lcf.h>
#include <lcom/timer.h>

static int hook_id = 0;

int(timer_subscribe_int)(uint8_t *bit_no) {
  hook_id = *bit_no;
  int buf = hook_id;
  int ret = sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &buf);
  if (ret != OK) {
    return ret;
  }
  *bit_no = buf;
  return OK;
}

int(timer_unsubscribe_int)() { return sys_irqrmpolicy(&hook_id); }

int timer_send_read_conf_cmd(uint8_t timer) {
  uint8_t cmd = TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer);
  return sys_outb(TIMER_CTRL, cmd);
}

int(timer_get_conf)(uint8_t timer, uint8_t *st) {
  if (timer_send_read_conf_cmd(timer) != OK) {
    return !OK;
  }

  if (util_sys_inb(TIMER_0 + timer, st) != OK) {
    return !OK;
  }

  return OK;
}

int(timer_display_conf)(uint8_t timer, uint8_t st,
                        enum timer_status_field field) {
  union timer_status_field_val val;

  switch (field) {
  case tsf_all:
    val.byte = st;
    break;
  case tsf_initial:
    st &= (BIT(5) | BIT(4));
    st >>= 4;
    val.in_mode = st;
    break;
  case tsf_mode:
    st &= (BIT(3) | BIT(2) | BIT(1));
    st >>= 1;
    val.count_mode = st;
    break;
  case tsf_base:
    st &= BIT(0);
    val.bcd = st;
    break;
  }

  return timer_print_config(timer, field, val);
}

int timer_send_set_freq_control_word(uint8_t timer) {
  uint8_t buf = 0;

  if (timer_get_conf(timer, &buf) != OK) {
    return !OK;
  }

  buf &= (BIT(3) | BIT(2) | BIT(1) | BIT(0));

  if (timer == 0) {
    buf |= TIMER_SEL0;
  } else if (timer == 1) {
    buf |= TIMER_SEL1;
  } else {
    buf |= TIMER_SEL2;
  }

  if (sys_outb(TIMER_CTRL, buf) != OK) {
    perror("sys_outb");
    return !OK;
  }

  return OK;
}

int(timer_set_frequency)(uint8_t timer, uint32_t frequency) {
  if (timer_send_set_freq_control_word(timer) != OK) {
    return !OK;
  }

  uint32_t counter = TIMER_FREQ / frequency;
  uint8_t msb, lsb;
  util_get_MSB(counter, &msb);
  util_get_LSB(counter, &lsb);

  // Assuming LSB followed by MSB
  // To be on the safe side, one should check the config and switch
  // appropriately
  if (sys_outb(TIMER_0 + timer, lsb) != OK) {
    perror("sys_outb lsb");
    return !OK;
  }

  if (sys_outb(TIMER_0 + timer, msb) != OK) {
    perror("sys_outb msb");
    return !OK;
  }

  return OK;
}
