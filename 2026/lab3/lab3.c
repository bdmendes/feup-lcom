#include <lcom/lab3.h>
#include <lcom/lcf.h>
#include <minix/syslib.h>
#include <minix/sysutil.h>

#define KBC_OBF_PORT 0x60
#define KBC_IBF_ARG_PORT 0x60
#define KBC_ST_REG_PORT 0x64
#define KBC_IBF_CMD_PORT 0x64

int main(int argc, char *argv[]) {
  lcf_set_language("EN-US");
  lcf_trace_calls("/home/lcom/labs/lab3/trace.txt");
  lcf_log_output("/home/lcom/labs/lab3/output.txt");
  lcf_start(argc, argv);
  return lcf_cleanup();
}

uint8_t codes[2];
int size = 1;
int seen_flag = 0;
int skipped = 0;

void (kbc_ih)(void) {
    uint32_t st;
    uint32_t code;
    sys_inb(KBC_ST_REG_PORT, &st);
    if (!(st & BIT(0))) {
        skipped = 1;
        return;
    } else {
        skipped = false;
    }
    sys_inb(KBC_OBF_PORT, &code);
    if (st & (BIT(7) | BIT(6))) {
        printf("parity error\n");
        return;
    }
    if (code == 0xE0) {
        seen_flag = 1;
        codes[0] = code;
        size = 1;
    } else if (seen_flag) {
        codes[1] = code;
        seen_flag = 0;
        size = 2;
    } else {
        codes[0] = code;
        seen_flag = 0;
        size = 1;
    }
}

int(kbd_test_scan)() {
    int ipc_status, r;
    message msg;

    int hook_id = 3;
    int irq_set = BIT(hook_id);
    sys_irqsetpolicy(1, IRQ_EXCLUSIVE | IRQ_REENABLE, &hook_id);
    int stop = 0;

    while( !stop ) { /* You may want to use a different condition */
        /* Get a request message. */
        if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
            printf("driver_receive failed with: %d", r);
            continue;
        }
        if (is_ipc_notify(ipc_status)) { /* received notification */
            switch (_ENDPOINT_P(msg.m_source)) {
                case HARDWARE: /* hardware interrupt notification */
                    if (msg.m_notify.interrupts & irq_set) { /* subscribed interrupt */
                        kbc_ih();
                        if (!seen_flag) {
                            kbd_print_scancode(!(codes[size-1] & BIT(7)), size, codes);
                        }
                        if (codes[size-1] == 0x81) {
                            stop = 1;
                        }
                    }
                    break;
                default:
                    break; /* no other notifications expected: do nothing */
            }
        } else { /* received a standard message, not a notification */
            /* no standard messages expected: do nothing */
        }
    }

    return sys_irqdisable(&hook_id);
}

int(kbd_test_poll)() {
    codes[size -1] = 0;
    while (codes[size-1] != 0x81) {
        kbc_ih();
        if (!skipped && !seen_flag) {
            kbd_print_scancode(!(codes[size-1] & BIT(7)), size, codes);
        }
        tickdelay(micros_to_ticks(20000));
    }

    sys_outb(KBC_IBF_CMD_PORT, 0x20);
    uint32_t cmd_byte;
    sys_inb(KBC_OBF_PORT, &cmd_byte);

    cmd_byte |= BIT(0);

    sys_outb(KBC_IBF_CMD_PORT, 0x60);
    tickdelay(micros_to_ticks(20000));
    return sys_outb(KBC_IBF_ARG_PORT, cmd_byte);
}

int(kbd_test_timed_scan)(uint8_t n) {
  return 0;
}
