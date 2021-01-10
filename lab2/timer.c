#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include <i8254.h>

uint16_t timer_count = 0;
int hook_id;  

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  // change the config (read back)
  uint8_t current_conf = 0;
  timer_get_conf(timer,&current_conf);
  uint8_t lsb = 0x0F & current_conf;
  uint8_t new_conf = (timer << 6) | TIMER_LSB_MSB | lsb;
  sys_outb(TIMER_CTRL,new_conf);

  // write frequency in count register
  uint16_t count = TIMER_FREQ / freq;
  uint8_t lsb_count, msb_count;
  util_get_LSB(count,&lsb_count);
  util_get_MSB(count,&msb_count);

  // write lsb followed by msb
  return sys_outb(TIMER_0 + timer,lsb_count)
   & sys_outb(TIMER_0 + timer, msb_count);
}

int (timer_subscribe_int)(uint8_t *bit_no) {

    hook_id = *bit_no;
    if(sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_id) != OK) {
      printf("sys_irqsetpolicy failed");
      return 1;
    }

    return 0;
}

int (timer_unsubscribe_int)() {
  
   if (sys_irqrmpolicy(&hook_id) != OK) {
     printf("sys_irqmpolicy failed");
     return 1;
   }

  return 0;
}


void (timer_int_handler)() {

  timer_count ++;

}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {

  // status = 0 so count needs to be 1
  uint8_t config_word = TIMER_RB_CMD |
  TIMER_RB_COUNT_ | TIMER_RB_SEL(timer);

  if (sys_outb(TIMER_CTRL,config_word) != OK){
    ERROR_OUT;
  }
  return util_sys_inb(TIMER_0 + timer, st);
}

int (timer_display_conf)(uint8_t timer, uint8_t st,
                        enum timer_status_field field) {

  union timer_status_field_val value;
  
  switch (field){
    case tsf_all:
      value.byte = st;
      break;
    case tsf_initial:
      value.in_mode = CTRL_INITIALIZATION_MODE & st;
      value.in_mode >>= 4;
      break;
    case tsf_mode:
      value.count_mode = CTRL_OPERATING_MODE & st;
      value.count_mode >>= 1;
      break;
    case tsf_base:
      value.bcd = CTRL_COUNTING_BASE & st;
      break;
  }

  return timer_print_config(timer,field,value);
}
