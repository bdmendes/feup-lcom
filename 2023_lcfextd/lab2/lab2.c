#include "../common/lib.h"
#include "i8254.h"

static int counter = 0;
static int max_time = 0;

static void timer_ih() {
  counter += 1;
  if (counter % 60 == 0) {
    timer_print_elapsed_time();
    max_time -= 1;
  }
  if (max_time == 0) {
    exit_interrupt_loop();
  }
}

int(timer_test_read_config)(uint8_t timer, enum timer_status_field field) {
  uint8_t st;
  timer_get_conf(timer, &st);
  timer_display_conf(timer, st, field);
  return 0;
}

int(timer_test_time_base)(uint8_t timer, uint32_t freq) {
  return timer_set_frequency(timer, freq);
}

int(timer_test_int)(uint8_t time) {
  max_time = time;
  subscribe_device_interrupts(TIMER0_IRQ, IRQ_REENABLE, timer_ih);
  interrupt_loop();
  unsubscribe_device_interrupts(TIMER0_IRQ);
  return 0;
}
