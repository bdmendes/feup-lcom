#include "kbc.h"

static uint8_t buffer_data = 0x0;

int kbc_get_status(uint8_t* status){
  return util_sys_inb(KBC_STAT_REGISTER, status);
}

int (kbc_can_write_to_inbuf)() {
  // read the status byte
  uint8_t status_byte = 0;
  kbc_get_status(&status_byte);

  // check if the input buffer is full
  if (status_byte & IBF) {
    printf("Could not write command byte because the input buffer is full\n");
    return !OK;
  }
  return OK;
}


int (read_buffer_data)(bool aux_set){

  uint8_t status = 0;
  if (kbc_get_status(&status) != OK){
    printf("error reading status register!\n");
    return !OK;
  }

  if (!(status & OBF)){
    return !OK;
  }

  if (!aux_set && (status & ST_AUX)){
    printf("status aux bit is not cleared!\n");
    return !OK;
  }

  if (aux_set && !(status & ST_AUX)){
    printf("status aux bit is cleared!\n");
    return !OK;
  }

  if ((status & ST_PARITY) || (status & ST_TIMEOUT)){
    printf("parity/timeout error!\n");
    return !OK;
  }

  if (util_sys_inb(KBC_OUT_BUF, &buffer_data) != OK){
    printf("error reading scan code from output buffer!\n");
    return !OK;
  }

  return OK;
}

uint8_t (get_buffer_data)(){
  return buffer_data;
}
