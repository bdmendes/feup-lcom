// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>

#include "kbc.h"
#include "i8254.h"

extern uint16_t timer_count;

// state machines
typedef enum {INIT_MOUSE, LB_DOWN, RB_DOWN, INV} mouse_state;
typedef enum {INIT_DRAW, DRAWING_LEFT, LEFT_COMPLETE, DRAWING_RIGHT, COMPLETE} drawing_state;
mouse_state m_st;
drawing_state d_st;

// mouse data storage
uint8_t packet_data[3];
uint8_t counter = 0;
extern uint8_t buffer_data;

void update_mouse_state(struct packet pckt){

  // both buttons must not be pressed at the same time
  if (pckt.lb && pckt.rb){
    m_st = INV;
    return;
  }

  switch (m_st){
    case INIT_MOUSE:
      if (pckt.lb) m_st = LB_DOWN;
      else if (pckt.rb) m_st = RB_DOWN;
      break;
    case LB_DOWN:
      if (pckt.rb) m_st = INV;
      else if (!pckt.lb) m_st = INIT_MOUSE;
      break;
    case RB_DOWN:
      if (pckt.lb) m_st = INV;
      else if (!pckt.rb) m_st = INIT_MOUSE;
      break;
    case INV:
      if (!pckt.lb && !pckt.rb) m_st = INIT_MOUSE;
      break;
  }
}

void (update_drawing_state)(struct packet pckt, uint8_t x_len, uint8_t tolerance, uint16_t* x_delta) {

  if (m_st == INV){
    d_st = INIT_DRAW;
    *x_delta = 0;
    return;
  }

  // when conditions arent met, forces mouse state to invalid
  // so that user has to release and start again
  switch (d_st){
    case INIT_DRAW:
      if (m_st == LB_DOWN){
        d_st = DRAWING_LEFT;
        *x_delta = 0;
      }
      break;

    case DRAWING_LEFT:
      if (pckt.delta_x < -tolerance || pckt.delta_y < -tolerance || m_st == RB_DOWN){
        d_st = INIT_DRAW;
        m_st = INV;
        *x_delta = 0;
      }
      else {
        *x_delta += pckt.delta_x;
        if (m_st == INIT_MOUSE){
          if (*x_delta >= x_len){
            d_st = LEFT_COMPLETE;
            *x_delta = 0;
          }
          else d_st = INIT_DRAW;
        }
      }
      break;

    case LEFT_COMPLETE:
      *x_delta = 0;
      if (abs(pckt.delta_x) > tolerance || abs(pckt.delta_y) > tolerance){
        d_st = INIT_DRAW;
        m_st = INV;
      }
      else if (m_st == LB_DOWN) d_st = DRAWING_LEFT;
      else if (m_st == RB_DOWN) d_st = DRAWING_RIGHT;
      break;

    case DRAWING_RIGHT:
      if (pckt.delta_x < -tolerance || pckt.delta_y > tolerance || m_st == LB_DOWN){
        d_st = INIT_DRAW;
        m_st = INV;
        *x_delta = 0;
      }
      else {
        *x_delta += pckt.delta_x;
        if (m_st == INIT_MOUSE){
          if (*x_delta >= x_len){
            d_st = COMPLETE;
            *x_delta = 0;
          }
          else d_st = INIT_DRAW;
        }
      }
      break;

    case COMPLETE:
      break;
  }
}

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab4/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab4/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int (mouse_test_packet)(uint32_t cnt) {

  kbc_mouse_enable_data_reporting();

  uint8_t bit_no = MOUSE_IRQ; // must be initialized to something!
  int ipc_status;
  message msg;
  int r;

  bool is_synced = false;

  mouse_subscribe_int(&bit_no);
  uint32_t irq_set = BIT(bit_no);

  while(cnt>0) { 
    /* Get a request message. */
    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) { /* received notification */
      if (_ENDPOINT_P(msg.m_source) == HARDWARE) { /* hardware interrupt notification */	
        if (msg.m_notify.interrupts & irq_set) { /* subscribed interrupt */

          mouse_ih();

          // check if it can't be the first byte
          if (!is_synced && (buffer_data & MOUSE_BYTE1_FLAG)){
            counter = 0;
            is_synced = true;
          }

          if (!is_synced) continue;
          else{
            packet_data[counter] = buffer_data;
            counter++;
          }

          if (counter == 3){
            counter = 0;
            cnt--;
            struct packet pckt = generate_packet(packet_data);
            mouse_print_packet(&pckt);
            fflush(stdout);
          }
        }
      }
    }
  }

  if (mouse_unsubscribe_int() != 0) {
    printf("error unsubscribing mouse interrupt!\n");
    return !OK;
  }

  return kbc_mouse_disable_data_reporting();
}

int (mouse_test_async)(uint8_t idle_time) {

  kbc_mouse_enable_data_reporting();

  uint8_t mouse_bit_no = MOUSE_IRQ; // must be initialized to something!
  uint8_t timer_bit_no = TIMER0_IRQ;

  int ipc_status;
  message msg;
  int r;

  bool is_synced = false;

  mouse_subscribe_int(&mouse_bit_no);
  timer_subscribe_int(&timer_bit_no);
  uint32_t mouse_irq_set = BIT(mouse_bit_no);
  uint8_t timer_irq_set = BIT(timer_bit_no);

  while (timer_count/60 < idle_time) {

    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
        printf("driver_receive failed with: %d", r);
        continue;
    }

    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: /*hardware interrupt notification*/

          if (msg.m_notify.interrupts & mouse_irq_set) { // mouse int?

            mouse_ih();

            // check if it can't be the first byte
            if (!is_synced && (buffer_data & MOUSE_BYTE1_FLAG)){
              counter = 0;
              is_synced = true;
            }

            if (!is_synced) continue;
            else{
              packet_data[counter] = buffer_data;
              counter++;
            }

            if (counter == 3){
              counter = 0;
              struct packet pckt = generate_packet(packet_data);
              mouse_print_packet(&pckt);
              fflush(stdout);
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

  if (mouse_unsubscribe_int() != 0) {
    printf("error unsubscribing mouse interrupt!\n");
    return !OK;
  }

  if (timer_unsubscribe_int() != 0) {
    printf("error unsubscribing timer interrupt!\n");
    return !OK;
 }

  return kbc_mouse_disable_data_reporting();
}

int (mouse_test_gesture)(uint8_t x_len, uint8_t tolerance) {
  kbc_mouse_enable_data_reporting();

  uint8_t bit_no = MOUSE_IRQ; // must be initialized to something!
  int ipc_status; 
  message msg;
  int r;
  bool is_synced = false;

  mouse_subscribe_int(&bit_no);
  uint32_t irq_set = BIT(bit_no);

  m_st = INIT_MOUSE;
  d_st = INIT_DRAW;

  uint16_t x_delta = 0;

  while(d_st != COMPLETE) {
    /* Get a request message. */
    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) { /* received notification */
      if (_ENDPOINT_P(msg.m_source) == HARDWARE) { /* hardware interrupt notification */	
        if (msg.m_notify.interrupts & irq_set) { /* subscribed interrupt */

          mouse_ih();

          // check if it can't be the first byte
          if (!is_synced && (buffer_data & MOUSE_BYTE1_FLAG)){
            counter = 0;
            is_synced = true;
          }

          if (!is_synced) continue;
          else{
            packet_data[counter] = buffer_data;
            counter++;
          }

          if (counter == 3){
            counter = 0;
            struct packet pckt = generate_packet(packet_data);
            mouse_print_packet(&pckt);
            fflush(stdout);

            //update state machines
            update_mouse_state(pckt);
            update_drawing_state(pckt,x_len,tolerance,&x_delta);
            printf("mouse state: %d, draw state: %d\n",m_st,d_st);
          }
        }
      }
    }
  }

  if (mouse_unsubscribe_int() != 0) {
    printf("error unsubscribing mouse interrupt!\n");
    return !OK;
  }

  return kbc_mouse_disable_data_reporting();
}

int (mouse_test_remote)(uint16_t period, uint8_t cnt) {

  bool is_synced = false;

  while (cnt > 0) {

    if (counter == 0 && kbc_write_mouse_byte(MOUSE_COMMAND_READ_DATA) != OK){
      printf("error writing command byte!");
    }

    if (mouse_ph() != OK){
      delay_milli_seconds(period);
      continue;
    }

    // check if it can't be the first byte
    if (!is_synced && (buffer_data & MOUSE_BYTE1_FLAG)){
      counter = 0;
      is_synced = true;
    }

    if (!is_synced) continue;
    else{
      packet_data[counter] = buffer_data;
      counter++;
    }

    if (counter == 3){
      counter = 0;
      cnt--;
      struct packet pckt = generate_packet(packet_data);
      mouse_print_packet(&pckt);
      fflush(stdout);
      delay_milli_seconds(period);
    }
    
  }

  kbc_write_mouse_byte(MOUSE_COMMAND_SET_STREAM_MODE);
  kbc_mouse_disable_data_reporting();

  uint8_t default_command_byte = minix_get_dflt_kbc_cmd_byte();
  return kbc_write_command_byte(default_command_byte);
}
