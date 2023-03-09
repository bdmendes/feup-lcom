#include <lcom/lcf.h>

#include "kbc.h"
#include "mouse.h"

#define MAX_TRIES 3
#define delay() tickdelay(micros_to_ticks(20000))

static int hook_id = MOUSE_IRQ;
static uint8_t mouse_packet[] = {0, 0, 0};
static uint8_t packet_index = 0;

uint8_t *mouse_get_packet_byte() { return mouse_packet; }

uint8_t mouse_get_packet_index() { return packet_index; }

int mouse_subscribe_int(uint8_t *bit_no) {
  *bit_no = MOUSE_IRQ;
  if (sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id) !=
      OK) {
    printf("Error setting policy");
    return 1;
  }
  return 0;
}

int mouse_unsubscribe_int() {
  if (sys_irqrmpolicy(&hook_id) != OK) {
    printf("Error removing policy");
    return 1;
  }
  return 0;
}

int mouse_write_mouse_byte(uint8_t mouse_byte) {
  int tries = 0;
  uint8_t ack_byte = 0;
  uint8_t status;

  while (tries++ < MAX_TRIES) {
    for(;;) {
      if (util_sys_inb(KBC_STAT_REG, &status) != OK) {
        printf("Error reading status register");
        return 1;
      }
      if (status & KBC_STAT_IBF) {
        delay();
        continue;
      }
      break;
    }

    if (kbc_write_cmd(KBD_WRITE_BYTE_MOUSE)) {
      printf("Error writing mouse byte cmd");
      return 1;
    }

    for(;;) {
      if (util_sys_inb(KBC_STAT_REG, &status) != OK) {
        printf("Error reading status register");
        return 1;
      }
      if (status & KBC_STAT_IBF) {
        delay();
        continue;
      }
      break;
    }

    if (sys_outb(KBC_IN_BUF, mouse_byte) != OK) {
      printf("Error writing mouse byte arg");
      return 1;
    }

    if (kbc_read_obf(&ack_byte) != OK) {
      printf("Error reading output buffer");
      return 1;
    }

    if (ack_byte == MOUSE_ACK) {
      break;
    }
    delay();
  }

  if (tries == MAX_TRIES) {
    printf("Error writing mouse byte");
    return 1;
  }

  return 0;
}

void mouse_read() {
  uint8_t byte;
  if (kbc_read_obf(&byte) != OK) {
    printf("Error reading output buffer");
    return;
  }

  // Sync first byte
  if (packet_index == 0 && !(byte & BIT(3))) {
    return;
  }

  mouse_packet[packet_index++] = byte;

  if (packet_index == 3) {
    packet_index = 0;
  }
}

void(mouse_ih)() { mouse_read(); }

struct packet mouse_assemble_packet() {
  struct packet p;

  if (packet_index != 0) {
    printf("Error assembling packet");
    return p;
  }

  for (int i = 0; i < 3; i++) {
    p.bytes[i] = mouse_packet[i];
  }

  p.rb = p.bytes[0] & MOUSE_RB;
  p.mb = p.bytes[0] & MOUSE_MB;
  p.lb = p.bytes[0] & MOUSE_LB;

  p.delta_x = p.bytes[1];
  if (p.bytes[0] & MOUSE_XSIGN) {
    p.delta_x |= 0xFF00;
  }

  p.delta_y = p.bytes[2];
  if (p.bytes[0] & MOUSE_YSIGN) {
    p.delta_y |= 0xFF00;
  }

  p.x_ov = p.bytes[0] & MOUSE_XOV;
  p.y_ov = p.bytes[0] & MOUSE_YOV;

  return p;
}