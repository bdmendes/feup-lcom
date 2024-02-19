#include <lcom/lcf.h>

#include <lcom/lab3.h>

#include "kbc.h"

#include "i8254.h"

#include <stdbool.h>
#include <stdint.h>

extern uint8_t buffer_data;
extern int sys_inb_count;
extern int timer_count;

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
  uint8_t bytes[2];
  int index = 0;

  uint8_t bit_no = KDB_IRQ;
  kbd_subscribe_int(&bit_no);

  while( buffer_data != ESC_BREAKCODE ){
    if( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ){
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) { /*received notification*/
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: /*hardware interrupt notification*/
          if (msg.m_notify.interrupts & bit_no) { /*subscribed interrupt*/
            
            kbd_ih();

            bytes[index] = buffer_data;
            if (buffer_data == FIRST_OF_TWO_BYTES){
              index++;
              continue;
            }
            else {
              kbd_print_scancode(is_make_code(buffer_data), index+1, bytes);
              index = 0;
            }
          }
          break;
        default:
          break; /*no other notifications expected: do nothing*/
        }
    }
    else { /*received a standard message, not a notification*/
      /*no standard messages expected: do nothing*/ 
    } 
  }

  if (kbd_unsubscribe_int() != OK){
    printf("error unsubscribing kbd int");
  }

  return kbd_print_no_sysinb(sys_inb_count);
}

int(kbd_test_poll)() {
  uint8_t bytes[2];
  int index = 0;

  while( buffer_data != ESC_BREAKCODE) {

    if (read_buffer_data() != OK){
      delay_milliseconds(DELAY_MILLISECONDS);
      continue;
    }
    
    bytes[index] = buffer_data;
    if (buffer_data == FIRST_OF_TWO_BYTES){
      index++;
      continue;
    }
    else {
      kbd_print_scancode(is_make_code(buffer_data), index+1, bytes);
      index = 0;
    }
  }

  if (kbd_reenable_interrupts() != OK){
    printf("error reenabling interrupts");
  }

  return kbd_print_no_sysinb(sys_inb_count);
}

int(kbd_test_timed_scan)(uint8_t n) {
  int r;
  int ipc_status;
  message msg;
  uint8_t bytes[2];
  int index = 0;

  uint8_t kbd_bit_no = KDB_IRQ;
  uint8_t timer_bit_no = TIMER0_IRQ;
  kbd_subscribe_int(&kbd_bit_no);
  timer_subscribe_int(&timer_bit_no);

  while( buffer_data != ESC_BREAKCODE ){
    if( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ){
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) { /*received notification*/
      if (_ENDPOINT_P(msg.m_source) == HARDWARE) {

        if (msg.m_notify.interrupts & kbd_bit_no) { /*subscribed interrupt*/
          
          timer_count = 0;
          kbd_ih();

          bytes[index] = buffer_data;
          if (buffer_data == FIRST_OF_TWO_BYTES){
            index++;
            continue;
          }
          else {
            kbd_print_scancode(is_make_code(buffer_data), index+1, bytes);
            index = 0;
          }
        }

        if (msg.m_notify.interrupts & timer_bit_no){
          timer_int_handler();
          if (timer_count / 60 >= n) break;
        }
      }
    }
  }

  if (kbd_unsubscribe_int() != OK){
    printf("error unsubscribing kbd int");
  }

  if (timer_unsubscribe_int() != OK){
    printf("error unsubscring timer int");
  }

  return OK;
}
