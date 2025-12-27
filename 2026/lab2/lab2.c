#include "lcom/timer.h"
#include "lcom/utils.h"
#include "minix/com.h"
#include "minix/syslib.h"
#include <lcom/lab2.h>
#include <lcom/lcf.h>

#include <stdbool.h>
#include <stdint.h>

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab2/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab2/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int util_sys_inb(int port, uint8_t *value) {
    uint32_t val;
    int ret = sys_inb(port, &val);
    *value = val;
    return ret;
}

int timer_read_conf(uint8_t timer, uint8_t *buf) {
  uint8_t rdb_cmd = BIT(7) | BIT(6) | BIT(5) | BIT(timer + 1);
  sys_outb(0x43, rdb_cmd);
  return util_sys_inb(0x40 + timer, buf);
}

int(timer_test_read_config)(uint8_t timer, enum timer_status_field field) {
  uint8_t conf;
  timer_read_conf(timer, &conf);

  union timer_status_field_val val;
  switch (field) {
  case tsf_all:
    val.byte = conf;
    break;
  case tsf_initial:
    val.in_mode = (conf >> 4) & 3;
    break;
  case tsf_mode:
    val.count_mode = (conf >> 1) & 7;
    if (val.count_mode == 6)
      val.count_mode = 2;
    if (val.count_mode == 7)
      val.count_mode = 3;
    break;
  case tsf_base:
    val.bcd = conf & 1;
    break;
  }

  timer_print_config(timer, field, val);

  return 1;
}

int(timer_test_time_base)(uint8_t timer, uint32_t freq) {
  uint8_t conf;
  timer_read_conf(timer, &conf);

  uint8_t ctrl_word = conf & (BIT(0) | BIT(1) | BIT(2) | BIT(3));
  ctrl_word |= timer << 6;
  ctrl_word |= 0x3 << 4;
  sys_outb(0x43, ctrl_word);

  uint16_t initial_counter = 1193181 / freq;
  sys_outb(0x40 + timer, (uint8_t)initial_counter);
  sys_outb(0x40 + timer, (uint8_t)(initial_counter >> 8));

  return 1;
}

int(timer_test_int)(uint8_t time) {
  int hook_id = 3;
  uint64_t counter = 0;

  sys_irqsetpolicy(0, IRQ_REENABLE, &hook_id);

  int ipc_status, r;
  message msg;

  while (time > 0) { /* You may want to use a different condition */
              /* Get a request message. */
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
      case HARDWARE: /* hardware interrupt notification */
        if (msg.m_notify.interrupts & BIT(3)) { /* subscribed interrupt */
          counter++;
          if (counter % 60 == 0) { counter = 0;timer_print_elapsed_time(); time--;}
        }
        break;
      default:
        break; /* no other notifications expected: do nothing */
      }
    } else { /* received a standard message, not a notification */
             /* no standard messages expected: do nothing */
    }
  }

  sys_irqrmpolicy(&hook_id);

  return 1;
}
