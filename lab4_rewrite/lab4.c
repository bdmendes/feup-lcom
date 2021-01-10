// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>

#include "mouse.h"
#include "timer.h"

// Any header files included below this line should have been created by you

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab4_rewrite/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab4_rewrite/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

// mouse ih should read one byte per interrupt

int(mouse_test_packet)(uint32_t cnt) {
  
  uint8_t bit_no = MOUSE_IRQ;
  int ipc_status;
  int r;
  message msg;

  if (m_mouse_enable_data_reporting() != OK) {
    printf("Error enabling data reporting");
    return !OK;
  }

  if (mouse_subscribe_int(&bit_no) != OK) {
    return !OK;
  }

  int mouse_irq_set = BIT(bit_no);


  bool is_synced = false;
  uint8_t byte_counter = 0;
  uint8_t packet[3];

  while (cnt) {
    /* Get a request message. */
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:                                   /* hardware interrupt notification */
          if (msg.m_notify.interrupts & mouse_irq_set) { /* subscribed interrupt */
            mouse_ih();
            uint8_t buffer_data = get_buffer_data();
            if (!is_synced && (MOUSE_PCKT_FIRST_BYTE_FLAG & buffer_data)) {
              is_synced = true;
              byte_counter = 0;
            } 
            if (is_synced) {
              packet[byte_counter] = buffer_data;
              byte_counter++;
            } else {
              delay_milli_seconds(100);
              continue;
            }

            if (byte_counter == 3) {
              byte_counter = 0;
              struct packet pckt;
              mouse_assemble_packet(packet, &pckt);
              mouse_print_packet(&pckt);
              cnt --;
            }
            
          }
          
          break;
        default:
          break; /* no other notifications expected: do nothing */
      }
    }
    else { /* received a standard message, not a notification */
           /* no standard messages expected: do nothing */
    }
  }

  

  if (mouse_unsubscribe_int() != OK) {
    return !OK;
  }

  if (mouse_disable_data_reporting() != OK) {  // TODO: Implement disable_data_reporting
    printf("Error disabling data reporting");
    return !OK;
  }


  return OK;
}

int(mouse_test_async)(uint8_t idle_time) {

  uint8_t mouse_bit_no = MOUSE_IRQ;
  uint8_t timer_bit_no = TIMER0_IRQ;
  int ipc_status;
  int r;
  message msg;

  if (m_mouse_enable_data_reporting() != OK) {
    printf("Error enabling data reporting");
    return !OK;
  }

  if (mouse_subscribe_int(&mouse_bit_no) != OK) {
    return !OK;
  }

  if (timer_subscribe_int(&timer_bit_no) != OK) {
    return !OK;
  }

  int mouse_irq_set = BIT(mouse_bit_no);
  int timer_irq_set = BIT(timer_bit_no);


  bool is_synced = false;
  uint8_t byte_counter = 0;
  uint8_t packet[3];

  while (get_timer_count() < sys_hz()*idle_time) {
    /* Get a request message. */
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:                                   /* hardware interrupt notification */

          if (msg.m_notify.interrupts & timer_irq_set) { /* subscribed interrupt */
              timer_int_handler();
          }

          if (msg.m_notify.interrupts & mouse_irq_set) { /* subscribed interrupt */
            mouse_ih();
            uint8_t buffer_data = get_buffer_data();
            if (!is_synced && (MOUSE_PCKT_FIRST_BYTE_FLAG & buffer_data)) {
              is_synced = true;
              byte_counter = 0;
            } 
            if (is_synced) {
              packet[byte_counter] = buffer_data;
              byte_counter++;
            } else {
              delay_milli_seconds(100);
              continue;
            }

            if (byte_counter == 3) {
              byte_counter = 0;
              struct packet pckt;
              mouse_assemble_packet(packet, &pckt);
              mouse_print_packet(&pckt);
            }

            set_timer_count(0);
            
          }
          
          break;
        default:
          break; /* no other notifications expected: do nothing */
      }
    }
    else { /* received a standard message, not a notification */
           /* no standard messages expected: do nothing */
    }
  }

  

  if (mouse_unsubscribe_int() != OK) {
    return !OK;
  }

  if (timer_unsubscribe_int() != OK) {
    return !OK;
  }

  if (mouse_disable_data_reporting() != OK) {  // TODO: Implement disable_data_reporting
    printf("Error disabling data reporting");
    return !OK;
  }


  return OK;
}

int(mouse_test_gesture)(uint8_t x_len, uint8_t tolerance) {
  /* To be completed */
  printf("%s: under construction\n", __func__);
  return 1;
}

int(mouse_test_remote)(uint16_t period, uint8_t cnt) {
  /* To be completed */
  printf("%s(%u, %u): under construction\n", __func__, period, cnt);
  return 1;
}
