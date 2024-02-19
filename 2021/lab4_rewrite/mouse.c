#include "mouse.h"

static int mouse_hook_id = MOUSE_IRQ;

int(mouse_disable_data_reporting)() {
  if (mouse_write_command(MOUSE_DISABLE_DATA_REPORTING) != OK) {
    printf("Error disabling data reporting\n");
    return !OK;
  }
  return OK;
}


int(m_mouse_enable_data_reporting)() {

  if (mouse_write_command(MOUSE_ENABLE_DATA_REPORTING) != OK) {
    printf("Error disabling data reporting\n");
    return !OK;
  }
  return OK;
}

int (mouse_write_command)(uint8_t mouse_cmd_byte) {

  while (true) {

    while (kbc_can_write_to_inbuf() != OK) {
      delay_milli_seconds(DELAY_MILLISECONDS);
    }

    uint8_t mouse_ack_byte = 0;
    if (sys_outb(KBC_IN_BUF_COMMAND, KBC_WRITE_BYTE_TO_MOUSE) != OK) {
      printf("Error passing the write to mouse command\n");
      return !OK;
    }

    while (kbc_can_write_to_inbuf() != OK) {
      delay_milli_seconds(DELAY_MILLISECONDS);
    }

    if (sys_outb(KBC_IN_BUF_COMMAND_ARG, mouse_cmd_byte) != OK) {
      printf("Error passing the command to the mouse\n");
      return !OK;
    }

    if (util_sys_inb(KBC_OUT_BUF, &mouse_ack_byte) != OK) {
      printf("Error reading mouse ACK byte\n");
      return !OK;
    }

    if (mouse_ack_byte == MOUSE_ACK) {
      break;
    }

  }
  
  return OK;
}

int(mouse_subscribe_int)(uint8_t *bit_no) {
  *bit_no = mouse_hook_id;
  if (sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &mouse_hook_id)) {
    printf("Error during mouse interruption subscription\n");
    return !OK;
  }
  return OK;
}

int(mouse_unsubscribe_int)() {
  if (sys_irqrmpolicy(&mouse_hook_id)) {
    printf("Error during mouse interuption unsubscription\n");
    return !OK;
  }
  return OK;
}

void(mouse_assemble_packet)(uint8_t* data, struct packet *pp) {

  memcpy(pp->bytes, data, 3);

  (*pp).lb = data[0] & MOUSE_BYTE_1_LB;
  (*pp).mb = data[0] & MOUSE_BYTE_1_MB;
  (*pp).rb = data[0] & MOUSE_BYTE_1_RB;

  (*pp).delta_x = MOUSE_BYTE_1_X_DELTA_MSB & data[0] ? 0xFF00 | data[1] : data[1];
  (*pp).delta_y = MOUSE_BYTE_1_Y_DELTA_MSB & data[0] ? 0xFF00 | data[2] : data[2];

  (*pp).y_ov = data[0] & MOUSE_BYTE_1_Y_OVFL;
  (*pp).x_ov = data[0] & MOUSE_BYTE_1_X_OVFL;

}

void(mouse_ih)() {
  read_buffer_data(true);
}
