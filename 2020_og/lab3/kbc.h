#ifndef _LCOM_KBC_H
#define _LCOM_KBC_H

#include <lcom/lcf.h>

// kbd irq line
#define KBD_IRQ 1

// keyboard recommended delay
#define DELAY_US 20000

// buffers
#define OUT_BUF 0x60
#define STAT_REG 0x64
#define IN_BUF_KBC_COMMANDS 0x64
#define IN_BUF_KBC_ARGS 0x60

// outbuffer 
#define BREAK_CODE BIT(7)
#define TWO_BYTES_FLAG 0xE0

// stat reg
#define STAT_FULL_OUTPUT_BUFFER BIT(0)
#define STAT_FULL_INPUT_BUFFER BIT(1)
#define STAT_PARITY_ERROR BIT(7)
#define STAT_TIMEOUT_ERROR BIT(6)
#define STAT_AUX BIT(5) // if set, is mouse

// kbc commands
#define READ_COMMAND_BYTE 0x20
#define WRITE_COMMAND_BYTE 0x60
#define SELF_TEST_KBC 0xAA
#define CHECK_KBD_INTERFACE 0xAB
#define DISABLE_KBD_INTERFACE 0xAD
#define ENABLE_KDB_INTERFACE 0xAE

// self test returns
#define SELF_TEST_OK 0x55
#define SELF_TEST_ERROR 0xFC

// command byte
#define DIS2 BIT(5) //mouse
#define DIS BIT(4) //keyboard
#define INT2 BIT(1) //mouse
#define INT BIT(0) //keyboard

// key codes
#define ESC_BREAKCODE 0x81

// user defined functions
int (read_buffer_data)();
void (kbc_ih)();
int (kbd_subscribe_int)(uint8_t *bit_no);
int (kbd_unsubscribe_int)();
bool (is_make_code)(uint8_t code);
void (kbd_read_command_byte)(uint8_t* command_byte);
int (kbd_write_command_byte)(uint8_t command_byte);
bool (kbd_reenable_interruputs)();
int (kbc_ph)();
void (kbd_issue_command)(uint8_t command);
void (delay_ms)(unsigned micro_seconds);
int (kbc_get_status)(uint8_t* status);
bool (may_write_command_byte)(uint8_t status_byte);

#endif /*_LCOM_KBC_H*/
