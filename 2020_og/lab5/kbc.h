#ifndef KBC_H
#define KBC_H

#include <lcom/lcf.h>

#define KDB_IRQ 0x1
#define DELAY_MILLISECONDS 20

#define KBC_IN_BUF_COMMAND_ARG 0x60
#define KBC_IN_BUF_COMMAND 0x64
#define KBC_OUT_BUF 0x60
#define KBC_STAT_REGISTER 0x64

// status register
#define ST_PARITY BIT(7)
#define ST_TIMEOUT BIT(6)
#define ST_AUX BIT(5)
#define INH BIT(4)
#define IBF BIT(1)
#define OBF BIT(0)

// kbc commands
#define READ_COMMAND_BYTE 0x20
#define WRITE_COMMAND_BYTE 0x60

// command byte
#define INT BIT(0)

#define ESC_BREAKCODE 0x81
#define FIRST_OF_TWO_BYTES 0xE0

bool (is_make_code)(uint8_t code);
int (read_buffer_data)();
void (kbd_ih)();
int (kbd_ph)();
int (kbd_subscribe_int)(uint8_t* bit_no);
int (kbd_unsubscribe_int)();
int (kbd_reenable_interrupts)();
int wait_while_not_key(uint8_t key_breakcode);

#endif
