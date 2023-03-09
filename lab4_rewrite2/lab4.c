#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>

#include "mouse.h"
#include "kbc.h"

int main(int argc, char *argv[]) {
  lcf_set_language("EN-US");
  lcf_trace_calls("/home/lcom/labs/lab4_rewrite/trace.txt");
  lcf_log_output("/home/lcom/labs/lab4_rewrite/output.txt");

  if (lcf_start(argc, argv))
    return 1;

  lcf_cleanup();
  return 0;
}

int(mouse_test_packet)(uint32_t cnt) {
  uint8_t bit_no;
  int ipc_status;
  int r;
  message msg;

  // Enable data reporting
  if (mouse_write_mouse_byte(MOUSE_ENABLE_DATA_REPORTING)) {
    printf("Error disabling data reporting");
    return 1;
  }

  if (mouse_subscribe_int(&bit_no)) {
    printf("Error subscribing mouse interrupts");
    return 1;
  }

  bool synced = false;
  while (cnt > 0) {
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
      case HARDWARE:
        if (msg.m_notify.interrupts & BIT(bit_no)) {
          mouse_ih();
          if (mouse_get_packet_index() == 0) {
            if (!synced) {
              printf("Not synced...\n");
              continue;
            }
            cnt--;
            struct packet p = mouse_assemble_packet();
            mouse_print_packet(&p);
          } else if (!synced && mouse_get_packet_index() == 1){
            printf("Synced!\n");
            synced = true;
          }
        }
        break;
      default:
        break;
      }
    }
  }

  if (mouse_unsubscribe_int()) {
    printf("Error unsubscribing mouse interrupts");
    return 1;
  }

  // Disable data reporting
  if (mouse_write_mouse_byte(MOUSE_DISABLE_DATA_REPORTING)) {
    printf("Error disabling data reporting");
    return 1;
  }

  return OK;
}

int(mouse_test_remote)(uint16_t period, uint8_t cnt) {
  /* To be completed */
  printf("%s(%u, %u): under construction\n", __func__, period, cnt);
  return 1;
}

int(mouse_test_async)(uint8_t idle_time) {
  /* To be completed */
  printf("%s: under construction\n", __func__);
  return 1;
}
