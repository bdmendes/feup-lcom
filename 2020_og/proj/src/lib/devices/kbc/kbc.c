#include "kbc.h"
#include "../../utils/utils.h"

static uint8_t buffer_data = 0x0;

bool (kbc_can_write_to_inbuf)() {
  uint8_t status_byte = 0;
  if (util_sys_inb(KBC_STAT_REGISTER, &status_byte)){
    printf("error reading status register!\n");
    return false;
  }
  return !(status_byte & IBF);
}

int (read_buffer_data)(bool aux_set){

  uint8_t status_byte = 0;
  if (util_sys_inb(KBC_STAT_REGISTER, &status_byte)){
    printf("error reading status register!\n");
    return !OK;
  }

  if (!(status_byte & OBF)){
    return !OK;
  }

  if (!aux_set && (status_byte & ST_AUX)){
    printf("status aux bit is not cleared!\n");
    return !OK;
  }

  if (aux_set && !(status_byte & ST_AUX)){
    printf("status aux bit is cleared!\n");
    return !OK;
  }

  if ((status_byte & ST_PARITY) || (status_byte & ST_TIMEOUT)){
    printf("parity/timeout error!\n");
    return !OK;
  }

  if (util_sys_inb(KBC_OUT_BUF, &buffer_data) != OK){
    printf("error reading scan code from output buffer!\n");
    return !OK;
  }

  return OK;
}

inline uint8_t (get_buffer_data)(){
  return buffer_data;
}

int (kbc_read_command_byte)(uint8_t* cmd_byte){
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

int (kbc_write_command_byte)(uint8_t cmd_byte){

  while (!kbc_can_write_to_inbuf()) delay_milli_seconds(DEFAULT_DELAY_MILLISECONDS);
  if (sys_outb(KBC_IN_BUF_COMMAND, KBC_WRITE_COMMAND_BYTE) != OK) {
    printf("Error passing the write to mouse command\n");
    return !OK;
  }

  while (!kbc_can_write_to_inbuf()) delay_milli_seconds(DEFAULT_DELAY_MILLISECONDS);
  if (sys_outb(KBC_IN_BUF_COMMAND_ARG, cmd_byte) != OK) {
    printf("Error passing the command to the mouse\n");
    return !OK;
  }

  return OK;
}
