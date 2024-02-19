#include <lcom/lcf.h>

#include <kbc.h>

int kbd_hook_id;

uint8_t buffer_data; // current byte

int kbc_get_status(uint8_t* status){
  return util_sys_inb(STAT_REG, status);
}

int (read_buffer_data)(){
  uint8_t status = 0;
  if (kbc_get_status(&status) != OK){
    printf("error reading status register!\n");
    return !OK;
  }

  if (!(status & STAT_FULL_OUTPUT_BUFFER)){
    // this will repeat over and over on polls, don't print
    return !OK;
  }

  if (status & STAT_AUX){
    printf("status aux bit is not cleared!\n");
    return !OK;
  }

  if ((status & STAT_PARITY_ERROR) || (status & STAT_TIMEOUT_ERROR)){
    printf("parity/timeout error!\n");
    return !OK;
  }

  if (util_sys_inb(OUT_BUF, &buffer_data) != OK){
    printf("error reading scan code from output buffer!\n");
    return !OK;
  }

  return OK;
}

void (kbc_ih)(){
  read_buffer_data();
}

int (kbc_ph)(){ //poll handler
  return read_buffer_data();
}

int (kbd_subscribe_int)(uint8_t *bit_no) {
    kbd_hook_id = *bit_no;
    if(sys_irqsetpolicy(KBD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &kbd_hook_id) != OK) {
      printf("error subscribing keyboard interruption!\n");
      return !OK;
    }

    return OK;
}

int (kbd_unsubscribe_int)() {
  
   if (sys_irqrmpolicy(&kbd_hook_id) != OK) {
     printf("failed unsubscribing keyboard interruption!\n");
     return !OK;
   }

  return OK;
}

bool (is_make_code)(uint8_t code){
  return !(BREAK_CODE & code);
}

void (kbd_issue_command)(uint8_t command){
    if (sys_outb(IN_BUF_KBC_COMMANDS, command) != OK){
      printf("error issuing command to the kbc!\n");
    }
}

void (kbd_read_command_byte)(uint8_t* command_byte){
  //tell kbc we wanna read
  kbd_issue_command(READ_COMMAND_BYTE);

  if (util_sys_inb(OUT_BUF, command_byte) != OK){
    printf("error reading command byte!\n");
  }
}

int (kbd_write_command_byte)(uint8_t command_byte){

  // wait until we may write
  uint8_t status = 0;
  int remaining_tries = 5;
  for (;;){
    kbc_get_status(&status);
    if (!may_write_command_byte(status)){
      remaining_tries--;
      if (!remaining_tries) return !OK;
      else delay_ms(DELAY_US);
    }
    else break;
  }

  // tell kbc we wanna write
  kbd_issue_command(WRITE_COMMAND_BYTE);
  uint32_t long_command_byte = (uint32_t) command_byte;

  if (sys_outb(IN_BUF_KBC_ARGS, long_command_byte) != OK){
    printf("error writing command byte!\n");
    return !OK;
  }
  return OK;
}

bool (may_write_command_byte)(uint8_t status_byte){
  return !(status_byte & STAT_FULL_INPUT_BUFFER);
}

bool (kbd_reenable_interruputs)(){
  uint8_t current_command_byte;
  kbd_read_command_byte(&current_command_byte);
  current_command_byte |= INT; // enable for keyboard
  return kbd_write_command_byte(current_command_byte);
}

void (delay_ms)(unsigned micro_seconds){
  tickdelay(micros_to_ticks(micro_seconds));
}
