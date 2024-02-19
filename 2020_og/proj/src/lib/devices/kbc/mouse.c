#include "mouse.h"
#include "../../utils/utils.h"

static int mouse_hook_id = MOUSE_IRQ;
static uint8_t packet_bytes[3] = {0, 0, 0};
static uint8_t mouse_byte_counter = 0;

/**
 * @brief  assembles a mouse packet, filling the struct's fields
 * @param  pp: the packet to return the assembled result
 * @retval None
 */
static void (mouse_assemble_packet)(struct packet *pp);

int (mouse_disable_data_reporting)() {
  if (mouse_write_byte(MOUSE_DISABLE_DATA_REPORTING) != OK) {
    printf("Error disabling data reporting\n");
    return !OK;
  }
  return OK;
}

int (m_mouse_enable_data_reporting)() {
  if (mouse_write_byte(MOUSE_ENABLE_DATA_REPORTING) != OK) {
    printf("Error disabling data reporting\n");
    return !OK;
  }
  return OK;
}

int (mouse_write_byte)(uint8_t mouse_byte) {

  for (uint8_t mouse_response = 0; mouse_response != MOUSE_ACK;
   delay_milli_seconds(DEFAULT_DELAY_MILLISECONDS)) {

    while (!kbc_can_write_to_inbuf()) delay_milli_seconds(DEFAULT_DELAY_MILLISECONDS);
    if (sys_outb(KBC_IN_BUF_COMMAND, KBC_WRITE_BYTE_TO_MOUSE) != OK) {
      printf("Error passing the write to mouse command\n");
      return !OK;
    }

    while (!kbc_can_write_to_inbuf()) delay_milli_seconds(DEFAULT_DELAY_MILLISECONDS);
    if (sys_outb(KBC_IN_BUF_COMMAND_ARG, mouse_byte) != OK) {
      printf("Error passing the command to the mouse\n");
      return !OK;
    }

    if (util_sys_inb(KBC_OUT_BUF, &mouse_response) != OK) {
      printf("Error reading mouse ACK byte\n");
      return !OK;
    }

  }
  
  return OK;
}

int(mouse_subscribe_int)(uint8_t *bit_no) {
  *bit_no = mouse_hook_id;
  if (sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &mouse_hook_id) != OK) {
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

struct packet (get_mouse_packet)(){
  struct packet pp;
  memset(&pp,0,sizeof(struct packet));
  mouse_assemble_packet(&pp);
  return pp;
}

static void (mouse_assemble_packet)(struct packet *pp) {
  if (mouse_byte_counter != 3) return;

  memcpy(pp->bytes, packet_bytes, 3);

  pp->lb = packet_bytes[0] & MOUSE_BYTE_1_LB;
  pp->mb = packet_bytes[0] & MOUSE_BYTE_1_MB;
  pp->rb = packet_bytes[0] & MOUSE_BYTE_1_RB;

  pp->delta_x = MOUSE_BYTE_1_X_DELTA_MSB & packet_bytes[0] ? 0xFF00 | packet_bytes[1] : packet_bytes[1];
  pp->delta_y = MOUSE_BYTE_1_Y_DELTA_MSB & packet_bytes[0] ? 0xFF00 | packet_bytes[2] : packet_bytes[2];

  pp->y_ov = packet_bytes[0] & MOUSE_BYTE_1_Y_OVFL;
  pp->x_ov = packet_bytes[0] & MOUSE_BYTE_1_X_OVFL;

  mouse_byte_counter = 0;
}

void (mouse_ih)() {
  read_buffer_data(true);
  if (mouse_byte_counter == 0 && !(get_buffer_data() & MOUSE_PCKT_FIRST_BYTE_FLAG)) return;
  packet_bytes[mouse_byte_counter++] = get_buffer_data();
}

inline bool (mouse_packet_is_ready)(){
  return mouse_byte_counter == 3;
}
