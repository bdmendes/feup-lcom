#include <lcom/lcf.h>

#include <lcom/lab3.h>
#include "kbc.h"

#include "i8254.h"

#include <stdbool.h>
#include <stdint.h>

//count util_sys_inb calls
int count = 0;
extern uint16_t timer_count;

uint8_t out_buffer_data[2] = {0,0}; // both bytes if existing
extern uint8_t buffer_data; // current

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab3/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab3/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv)){
    return 1;
  }

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(kbd_test_scan)() {

  uint8_t bit_no = KBD_IRQ; // must be initialized to something!
  int ipc_status;
  message msg;
  int r;

  kbd_subscribe_int(&bit_no);
  uint8_t irq_set = BIT(bit_no);

  bool read_two_bytes = false;

  while(buffer_data != ESC_BREAKCODE) { 
    /* Get a request message. */
    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) { /* received notification */
      if (_ENDPOINT_P(msg.m_source) == HARDWARE) { /* hardware interrupt notification */	
        if (msg.m_notify.interrupts & irq_set) { /* subscribed interrupt */
          kbc_ih();
          out_buffer_data[0 + read_two_bytes] = buffer_data;

          if (buffer_data == TWO_BYTES_FLAG){
            read_two_bytes = true;
          }
          else {
            kbd_print_scancode(is_make_code(buffer_data), 1 + read_two_bytes, out_buffer_data);
            read_two_bytes = false;
          }

        }
      }
    }
  }

  if (kbd_unsubscribe_int() != 0) {
    printf("error unsubscribing keyboard interrupt!\n");
    return !OK;
  }

  return kbd_print_no_sysinb(count);
}

int(kbd_test_poll)(){

  bool read_two_bytes = false;

  while(buffer_data != ESC_BREAKCODE){ 

    if (kbc_ph() != OK){
      delay_ms(DELAY_US);
      continue;
    }

    out_buffer_data[0 + read_two_bytes] = buffer_data;

    if (buffer_data == TWO_BYTES_FLAG){
      read_two_bytes = true;
    }
    else {
      kbd_print_scancode(is_make_code(buffer_data), 1 + read_two_bytes, out_buffer_data);
      read_two_bytes = false;
    }

  }

  if (kbd_reenable_interruputs() != OK){
    printf("error reenabling interrupts!\n");
    return !OK;
  }

  return kbd_print_no_sysinb(count);
}

int(kbd_test_timed_scan)(uint8_t n) {

  bool read_two_bytes = false;

  uint8_t kbd_bit_no = KBD_IRQ; // must be initialized to something!
  uint8_t timer_bit_no = TIMER0_IRQ;

  int ipc_status;
  message msg;
  int r;

  timer_subscribe_int(&timer_bit_no);
  kbd_subscribe_int(&kbd_bit_no);

  uint8_t kbd_irq_set = BIT(kbd_bit_no);
  uint8_t timer_irq_set = BIT(timer_bit_no);

  while (buffer_data != ESC_BREAKCODE && !(timer_count/60 >= n)) {
    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
        printf("driver_receive failed with: %d", r);
        continue;
    }

    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: /*hardware interrupt notification*/

          if (msg.m_notify.interrupts & kbd_irq_set) { // KBD int?
                    /*process KBD interrupt request*/
              kbc_ih();
              out_buffer_data[0 + read_two_bytes] = buffer_data;

              if (buffer_data == TWO_BYTES_FLAG){
                read_two_bytes = true;
              }
              else {
                kbd_print_scancode(is_make_code(buffer_data), 1 + read_two_bytes, out_buffer_data);
                read_two_bytes = false;
              }

              timer_count = 0;

          }

          if (msg.m_notify.interrupts & timer_irq_set) { // Timer0 int?
              /*process Timer0 interrupt request*/
              timer_int_handler();
             
          }

          break;
        default:
          break; /*no other notifications expected: do nothing*/
      }
    }
  }

  if (kbd_unsubscribe_int() != 0) {
    printf("error unsubscribing keyboard interrupt!\n");
    return !OK;
  }

  if (timer_unsubscribe_int() != 0) {
    printf("error unsubscribing timer interrupt!\n");
    return !OK;
 }


  return OK;

}
