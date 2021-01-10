#include "keyboard.h"

static int kbd_hook_id = KDB_IRQ;

void (kbd_ih)(){
  read_buffer_data(false);
}

int (kbd_subscribe_int)(uint8_t* bit_no){
  *bit_no = kbd_hook_id;
  if (sys_irqsetpolicy(KDB_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &kbd_hook_id) != OK){
    printf("error subscribing kbd int\n");
    return !OK;
  }
  return OK;
}

int (kbd_unsubscribe_int)(){
  if (sys_irqrmpolicy(&kbd_hook_id) != OK){
    printf("error unsubscribing kbd int\n");
    return !OK;
  }
  return OK;
}

inline bool (is_make_code)(uint8_t code){
  return !(BIT(7) & code);
}

int (kbd_reenable_interrupts)(){
  uint8_t cmd_byte;
  kbc_read_command_byte(&cmd_byte);
  cmd_byte |= INT;
  return kbc_write_command_byte(cmd_byte);
}

inline uint8_t (get_break_code)(uint8_t make_code){
  return BIT(7) | make_code;
}
