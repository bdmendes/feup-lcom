#include "../common/lib.h"
#include "kbc.h"

// mouse
uint8_t bytes[3];
int idx = 0;
int mouse_packet_cnt = 0;

// timer
int timer_cnt = 0;
int timer_to_go = 0;
int og_timer_cnt = 0;

static void mouse_assemble_packet(struct packet *pkt) {
  memcpy(pkt->bytes, bytes, 3 * sizeof(uint8_t));
  pkt->delta_x = bytes[0] & MOUSE_BYTE1_MSBXDELTA ? bytes[1] & 0xFF : bytes[1];
  pkt->delta_y = bytes[0] & MOUSE_BYTE1_MSBYDELTA ? bytes[2] & 0xFF : bytes[2];
  pkt->lb = bytes[0] & MOUSE_BYTE1_LEFTBUTTON;
  pkt->rb = bytes[0] & MOUSE_BYTE1_RIGHTBUTTON;
  pkt->mb = bytes[0] & MOUSE_BYTE1_MIDDLEBUTTON;
  pkt->x_ov = bytes[0] & MOUSE_BYTE1_X_OVFL;
  pkt->y_ov = bytes[1] & MOUSE_BYTE1_Y_OVFL;
}

static void mouse_read_data(uint8_t *byte) {
  for (;;) {
    uint8_t st;
    util_sys_inb(KBC_STAT_REG, &st);
    if (st & KBC_STAT_OBF) {
      break;
    }
    sleep_abit();
  }

  util_sys_inb(KBC_OUT_BUF, byte);
}

void timer_ih() {
  timer_cnt += 1;
  if (timer_cnt % 60 == 0) {
    timer_to_go -= 1;
    if (timer_to_go == 0) {
      exit_interrupt_loop();
    }
  }
}

void(mouse_ih)() {
  timer_to_go = og_timer_cnt;

  uint8_t byte;
  mouse_read_data(&byte);

  if (idx == 0 && !(byte & MOUSE_BYTE1_FLAG)) {
    // Out of sync. Discard
    printf("out of sync...");
    return;
  }

  bytes[idx] = byte;

  if (idx == 2) {
    struct packet pkt;
    mouse_assemble_packet(&pkt);
    mouse_print_packet(&pkt);
    idx = 0;

    mouse_packet_cnt -= 1;
    if (mouse_packet_cnt == 0) {
      exit_interrupt_loop();
    }
  } else {
    idx += 1;
  }
}

int(mouse_test_packet)(uint32_t cnt) {
  mouse_packet_cnt = cnt;
  mouse_send_cmd(MOUSE_CMD_ARG_ENABLE_DATA_REPORTING);

  subscribe_device_interrupts(12, IRQ_EXCLUSIVE | IRQ_REENABLE, mouse_ih);
  interrupt_loop();
  unsubscribe_device_interrupts(12);

  mouse_send_cmd(MOUSE_CMD_ARG_DISABLE_DATA_REPORTING);
  return 0;
}

int(mouse_test_remote)(uint16_t period, uint8_t cnt) {
  mouse_send_cmd(MOUSE_CMD_ARG_DISABLE_DATA_REPORTING);

  while (cnt > 0) {
    mouse_send_cmd(MOUSE_CMD_ARG_READ_DATA);

    for (;;) {
      mouse_ih();
      if (idx == 0) {
        break;
      }
    }

    cnt -= 1;
    tickdelay(micros_to_ticks(period * 1000));
  }

  mouse_send_cmd(MOUSE_CMD_ARG_ENABLE_STREAM_MODE);

  mouse_send_cmd(MOUSE_CMD_ARG_DISABLE_DATA_REPORTING);

  sys_outb(KBC_IN_BUF_CMD, KBC_CMD_WRITE_BYTE);
  sleep_abit();
  sys_outb(KBC_IN_BUF_ARG, minix_get_dflt_kbc_cmd_byte());

  return 0;
}

int(mouse_test_async)(uint8_t idle_time) {
  og_timer_cnt = idle_time;
  timer_to_go = idle_time;
  mouse_send_cmd(MOUSE_CMD_ARG_ENABLE_DATA_REPORTING);

  subscribe_device_interrupts(12, IRQ_EXCLUSIVE | IRQ_REENABLE, mouse_ih);
  subscribe_device_interrupts(0, IRQ_REENABLE, timer_ih);
  interrupt_loop();
  unsubscribe_device_interrupts(12);
  unsubscribe_device_interrupts(0);

  mouse_send_cmd(MOUSE_CMD_ARG_DISABLE_DATA_REPORTING);
  return 0;
}
