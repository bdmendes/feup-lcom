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

bool (is_make_code)(uint8_t code){
  return !(BIT(7) & code);
}

int read_command_byte(uint8_t* cmd_byte){
  if (sys_outb(KBC_IN_BUF_COMMAND,READ_COMMAND_BYTE) != OK){
    printf("error attempting to read");
    return !OK;
  }

  if (util_sys_inb(KBC_OUT_BUF,cmd_byte) != OK){
    printf("error reading");
    return !OK;
  }
  return OK;
}

int write_command_byte(uint8_t cmd_byte){

  uint8_t st;

  /* check if buffer is full before writing */
  if (util_sys_inb(KBC_STAT_REGISTER, &st) != OK){
    printf("error reading stat register");
    return !OK;
  }
  if (st & IBF){
    printf("ibf full!");
    return !OK;
  }

  /* tell kbc we will write a command byte */
  if (sys_outb(KBC_IN_BUF_COMMAND,WRITE_COMMAND_BYTE) != OK){
    printf("error writing command");
    return !OK;
  } 

  /* check if buffer is full before writing */
  if (util_sys_inb(KBC_STAT_REGISTER, &st) != OK){
    printf("error reading stat register");
    return !OK;
  }
  if (st & IBF){
    printf("ibf full!");
    return !OK;
  }
  
  /* write the command byte */
  if (sys_outb(KBC_IN_BUF_COMMAND_ARG,cmd_byte) != OK){
    printf("error writing the command byte");
    return !OK;
  }

  return OK;
}

int (kbd_reenable_interrupts)(){
  uint8_t cmd_byte;
  read_command_byte(&cmd_byte);
  cmd_byte |= INT;
  return write_command_byte(cmd_byte);
}
