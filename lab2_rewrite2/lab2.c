#include <lcom/lab2.h>
#include <lcom/lcf.h>

#include <i8254.h>

int main(int argc, char **argv) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab2/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab2/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(timer_test_read_config)(uint8_t timer, enum timer_status_field field) {
  uint8_t status;

  if (timer_get_conf(timer, &status) != OK) {
    perror("timer_get_conf");
    return !OK;
  }

  if (timer_display_conf(timer, status, field) != OK) {
    perror("timer_display_conf");
    return !OK;
  }

  return OK;
}

int(timer_test_time_base)(uint8_t timer, uint32_t freq) {
  return timer_set_frequency(timer, freq);
}

int(timer_test_int)(uint8_t time) {
  uint8_t bit_no;
  int ipc_status;
  message msg;
  int r;

  timer_subscribe_int(&bit_no);
  uint8_t irq_set = BIT(bit_no);

  while (time > 0) {
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
      case HARDWARE:
        if (msg.m_notify.interrupts & irq_set) {
          timer_int_handler();
          uint32_t counter = timer_get_counter();
          if (counter % 60 == 0) {
            time -= 1;
            timer_print_elapsed_time();
          }
        }
        break;
      default:
        break; /* no other notifications expected: do nothing */
      }
    }
  }

  if (timer_unsubscribe_int() != 0) {
    perror("timer_unsubscribe_int");
    return 1;
  }

  return 0;
}
