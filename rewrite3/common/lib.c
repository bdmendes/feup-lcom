#include "lib.h"
#include "ih_vec.h"

static int current_bit_no = 0;
static ih_vec_t *vec;
static volatile bool should_exit_interrupt_loop = false;

int main(int argc, char *argv[]) {
  lcf_set_language("EN-US");

  lcf_trace_calls("/home/lcom/labs/trace.txt");

  lcf_log_output("/home/lcom/labs/output.txt");

  if (lcf_start(argc, argv))
    return 1;

  lcf_cleanup();

  return 0;
}

int util_sys_inb(int port, uint8_t *value) {
  uint32_t buf;
  int ret = sys_inb(port, &buf);
  *value = (uint8_t)buf;
  return ret;
}

void subscribe_device_interrupts(int irq_line, int policy, void (*func)(void)) {
  if (vec == NULL) {
    vec = ih_vec_create();
  }

  ih_entry entry;
  entry.irq_line = irq_line;
  entry.bit_no = current_bit_no;
  entry.hook_id = current_bit_no;
  entry.ih = func;

  if (sys_irqsetpolicy(irq_line, policy, &entry.hook_id) != OK) {
    panic("failed to set interrupt policy");
  }

  current_bit_no += 1;

  ih_vec_push(vec, entry);
}

void unsubscribe_device_interrupts(int irq_line) {
  for (int i = 0; i < ih_vec_size(vec); i++) {
    ih_entry entry = ih_vec_at(vec, i);

    if (entry.irq_line == irq_line) {
      ih_vec_remove_at(vec, i);

      if (sys_irqrmpolicy(&entry.hook_id) != OK) {
        panic("failed to remove policy");
      }

      break;
    }
  }
}

void exit_interrupt_loop() {
  should_exit_interrupt_loop = true;
  free(vec);
  vec = NULL;
}

void interrupt_loop() {
  int ipc_status;
  int r;
  message msg;

  while (!should_exit_interrupt_loop) {
    /* Get a request message. */
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed wi%d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
      case HARDWARE: /* hardware interrupt notification */
        for (int i = 0; i < ih_vec_size(vec); i++) {
          ih_entry entry = ih_vec_at(vec, i);
          if (msg.m_notify.interrupts & BIT(entry.bit_no)) {
            entry.ih();
          }
        }
        break;
      default:
        break; /* no other notifications expected: do nothing */
      }
    }
  }
}