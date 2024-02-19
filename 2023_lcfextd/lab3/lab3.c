#include "../common/lib.h"
#include "kbc.h"

static uint8_t bytes[2];
static int idx = 0;
static int timer_counter = 0;
static int max_time = 0;
static int og_max_time = 0;

void timer_ih() {
  timer_counter += 1;
  if (timer_counter % 60 == 0) {
    max_time -= 1;
  }
  if (max_time == 0) {
    exit_interrupt_loop();
  }
}

void kbd_ih() {
  max_time = og_max_time;

  uint8_t key;
  util_sys_inb(KBC_OUT_BUF, &key);

  if (key == KBD_FIRST_OF_TWO_BYTES) {
    idx += 1;
    return;
  } else if (key == KBD_ESC_KEY) {
    exit_interrupt_loop();
  }

  bytes[idx] = key;

  kbd_print_scancode(KBD_KEY_IS_MAKE(key), idx + 1, bytes);
  idx = 0;
}

void kbd_poll() {
  for (;;) {
    uint8_t st;
    util_sys_inb(KBC_STAT_REG, &st);

    if (st & KBC_STAT_OBF) {
      uint8_t key;
      util_sys_inb(KBC_OUT_BUF, &key);

      if (!(st & KBC_STAT_ERR)) {
        if (key == KBD_FIRST_OF_TWO_BYTES) {
          idx += 1;
          return;
        } else if (key == KBD_ESC_KEY) {
          return;
        }

        bytes[idx] = key;

        kbd_print_scancode(KBD_KEY_IS_MAKE(key), idx + 1, bytes);
        idx = 0;
      }
    } else {
      sleep_abit();
    }
  }
}

int(kbd_test_scan)() {
  subscribe_device_interrupts(KBD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, kbd_ih);
  interrupt_loop();
  unsubscribe_device_interrupts(KBD_IRQ);
  return 0;
}

int(kbd_test_poll)() {
  kbd_poll();
  enable_kbd_interrupts();
  return 0;
}

int(kbd_test_timed_scan)(uint8_t n) {
  og_max_time = n;
  max_time = n;
  subscribe_device_interrupts(0, IRQ_REENABLE, timer_ih);
  subscribe_device_interrupts(1, IRQ_REENABLE | IRQ_EXCLUSIVE, kbd_ih);
  interrupt_loop();
  unsubscribe_device_interrupts(1);
  unsubscribe_device_interrupts(0);
  return 0;
}
