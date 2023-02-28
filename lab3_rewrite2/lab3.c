#include "kbc.h"
#include <lcom/lab3.h>
#include <lcom/lcf.h>

#include <stdbool.h>
#include <stdint.h>

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't          t/need it]
  lcf_trace_calls("/home/lcom/labs/lab3/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab3/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(kbd_test_scan)() {
  int r;
  int ipc_status;
  message msg;
  uint8_t bit_no;

  if (kbc_subscribe_int(&bit_no) != OK) {
    printf("kbc_subscribe_int failed");
    return 1;
  }

  int byte_index = 0;
  int found_esc = 0;
  while (!found_esc) {
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) { /*received notification*/
      switch (_ENDPOINT_P(msg.m_source)) {
      case HARDWARE: /*hardware interrupt notification*/
        if (msg.m_notify.interrupts & BIT(bit_no)) { /*subscribed interrupt*/
          kbc_interrupt_handler();
          uint8_t *out_buf = kbc_get_out_buf();

          if (out_buf[byte_index] == ESC_BREAK_CODE) {
            found_esc = 1;
          }

          if (out_buf[byte_index] == TWO_BYTE_CODE) {
            byte_index++;
            continue;
          }

          int is_make_code = !IS_BREAK_CODE(out_buf[byte_index]);
          kbd_print_scancode(is_make_code, byte_index + 1, out_buf);

          byte_index = 0;
        }
        break;
      default:
        break; /*no other notifications expected: do nothing*/
      }
    }
  }

  if (kbc_unsubscibe_int() != OK) {
    printf("kbc_unsubscibe_int failed");
    return 1;
  }

  return 0;
}

int(kbd_test_poll)() {
  // this test function already disables the keyboard interrupts
  // if (kbc_disable_int() != OK) {
  //   printf("kbc_disable_int failed");
  //   return 1;
  // }

  int byte_index = 0;
  while (1) {
    if (kbc_read_byte() != OK) {
      micro_delay(5000);
      continue;
    }

    uint8_t *out_buf = kbc_get_out_buf();

    if (out_buf[byte_index] == TWO_BYTE_CODE) {
      byte_index++;
      continue;
    }

    int is_make_code = !IS_BREAK_CODE(out_buf[byte_index]);
    kbd_print_scancode(is_make_code, byte_index + 1, out_buf);

    if (out_buf[byte_index] == ESC_BREAK_CODE) {
      break;
    }

    byte_index = 0;
  }

  if (kbc_enable_int() != OK) {
    printf("kbc_enable_int failed");
    return 1;
  }

  return 0;
}

int(kbd_test_timed_scan)(uint8_t n) {
  printf("lab3.c::kbd_test_timed_scan(%d)\t", n);
  return 1;
}
