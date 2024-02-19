#include <lcom/lcf.h>

#include <kbc.h>

int kbd_hook_id;
int mouse_hook_id;

uint8_t buffer_data; // current byte

int kbc_get_status(uint8_t* status){
  return util_sys_inb(STAT_REG, status);
}

int (read_buffer_data)(bool aux_set){

  uint8_t status = 0;
  if (kbc_get_status(&status) != OK){
    printf("error reading status register!\n");
    return !OK;
  }

  if (!(status & STAT_FULL_OUTPUT_BUFFER)){
    return !OK;
  }

  if (!aux_set && (status & STAT_AUX)){
    printf("status aux bit is not cleared!\n");
    return !OK;
  }

  if (aux_set && !(status & STAT_AUX)){
    printf("status aux bit is cleared!\n");
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

void (kbd_ih)(){
  read_buffer_data(false);
}

int (kbd_ph)(){ //poll handler
  return read_buffer_data(false);
}

void (mouse_ih)(){
  read_buffer_data(true);
}

int (mouse_ph)(){
  return read_buffer_data(true);
}

int (mouse_subscribe_int)(uint8_t *bit_no){
    mouse_hook_id = *bit_no;
    if(sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &mouse_hook_id) != OK) {
      printf("error subscribing mouse interruption!\n");
      return !OK;
    }

    return OK;
}

int (mouse_unsubscribe_int)() {
   if (sys_irqrmpolicy(&mouse_hook_id) != OK) {
     printf("failed unsubscribing mouse interruption!\n");
     return !OK;
   }
  return OK;
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

void (kbc_issue_command)(uint8_t command){
    if (sys_outb(IN_BUF_KBC_COMMANDS, command) != OK){
      printf("error issuing command to the kbc!\n");
    }
}

void (kbc_read_command_byte)(uint8_t* command_byte){
  //tell kbc we wanna read
  kbc_issue_command(READ_COMMAND_BYTE);

  if (util_sys_inb(OUT_BUF, command_byte) != OK){
    printf("error reading command byte!\n");
  }
}

int (kbc_write_command_byte)(uint8_t command_byte){
  if (attempt_write_byte() != OK){
      printf("timeout attempting to write");
  }

  // tell kbc we wanna write
  kbc_issue_command(WRITE_COMMAND_BYTE);
  uint32_t long_command_byte = (uint32_t) command_byte;

  if (sys_outb(IN_BUF_KBC_ARGS, long_command_byte) != OK){
    printf("error writing command byte!\n");
    return !OK;
  }
  return OK;
}

int (kbc_write_mouse_byte)(uint8_t mouse_byte){
  // wait until we may write
  if (attempt_write_byte() != OK){
      printf("timeout attempting to write");
  }

  for(;;){
    uint8_t mouse_acknowledgment;

    // tell kbc we wanna write
    kbc_issue_command(WRITE_BYTE_MOUSE);

    // wait until we may write
    if (attempt_write_byte() != OK){
      printf("timeout attempting to write");
    }

    if (sys_outb(IN_BUF_KBC_ARGS, mouse_byte) != OK){
      printf("error writing command byte!\n");
    }
    
    if (util_sys_inb(OUT_BUF,&mouse_acknowledgment) != OK){
      printf("error reading out buffer\n");
    }
    if (mouse_acknowledgment == MOUSE_ACK) break;
  }
  return OK;
}

bool (may_write_command_byte)(uint8_t status_byte){
  return !(status_byte & STAT_FULL_INPUT_BUFFER);
}

bool (kbd_reenable_interruputs)(){
  uint8_t current_command_byte;
  kbc_read_command_byte(&current_command_byte);
  current_command_byte |= INT; // enable for keyboard
  return kbc_write_command_byte(current_command_byte);
}

void (delay_milli_seconds)(unsigned milli_seconds){
  tickdelay(micros_to_ticks(milli_seconds*1000));
}

void (delay_micro_seconds)(unsigned micro_seconds){
  tickdelay(micros_to_ticks(micro_seconds));
}

struct packet (generate_packet)(uint8_t* data){
  struct packet pckt;
  for(int idx = 0; idx < 3; ++idx)  
    pckt.bytes[idx] = data[idx];

  pckt.rb = data[0] & MOUSE_BYTE1_RB;
  pckt.mb = data[0] & MOUSE_BYTE1_MB;
  pckt.lb = data[0] & MOUSE_BYTE1_LB;

  if (data[0] & MOUSE_BYTE1_MSBXDELTA){
    pckt.delta_x = 0xFF00 | data[1];
  }
  else pckt.delta_x = data[1];

  if (data[0] & MOUSE_BYTE1_MSBYDELTA){
    pckt.delta_y = 0xFF00 | data[2];
  }
  else pckt.delta_y = data[2];

  pckt.x_ov = data[0] & MOUSE_BYTE1_XOVLF;
  pckt.y_ov = data[0] & MOUSE_BYTE1_YOVFL;
  return pckt;
}

int (kbc_mouse_disable_data_reporting)(){
  return kbc_write_mouse_byte(MOUSE_COMMAND_DISABLE_DATA_REPORTING);
}

int (kbc_mouse_enable_data_reporting)(){
  return kbc_write_mouse_byte(MOUSE_COMMAND_ENABLE_DATA_REPORTING);
}

int (attempt_write_byte)(){
  uint8_t status = 0;
  int remaining_tries = 5;
  for (;;){
    kbc_get_status(&status);
    if (!may_write_command_byte(status)){
      remaining_tries--;
      if (!remaining_tries) return !OK;
      else delay_micro_seconds(DELAY_US);
    }
    else return OK;
  }
}
