#ifndef _LCOM_KBC_H
#define _LCOM_KBC_H

#include <lcom/lcf.h>

#include <stdbool.h>

// kbd irq line
#define KBD_IRQ 1

// mouse irq line
#define MOUSE_IRQ 12

// kbc recommended delay
#define DELAY_US 20000

// buffers
#define OUT_BUF 0x60
#define STAT_REG 0x64
#define IN_BUF_KBC_COMMANDS 0x64
#define IN_BUF_KBC_ARGS 0x60

// kbd outbuffer 
#define BREAK_CODE BIT(7)
#define TWO_BYTES_FLAG 0xE0

// mouse outbuffer
#define MOUSE_BYTE1_YOVFL BIT(7)
#define MOUSE_BYTE1_XOVLF BIT(6)
#define MOUSE_BYTE1_MSBYDELTA BIT(5)
#define MOUSE_BYTE1_MSBXDELTA BIT(4)
#define MOUSE_BYTE1_FLAG BIT(3)
#define MOUSE_BYTE1_MB BIT(2)
#define MOUSE_BYTE1_RB BIT(1)
#define MOUSE_BYTE1_LB BIT(0)

// stat reg
#define STAT_FULL_OUTPUT_BUFFER BIT(0)
#define STAT_FULL_INPUT_BUFFER BIT(1)
#define STAT_INH BIT(4) // 0 if kbd is inhibited
#define STAT_A2 BIT(3) // 0 data byte, 1 command byte
#define STAT_SYS BIT(2) // 0 power-on reset, 1 system initialized
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
#define DISABLE_MOUSE 0xA7
#define ENABLE_MOUSE 0xA8
#define CHECK_MOUSE_INTERFACE 0xA9
#define WRITE_BYTE_MOUSE 0xD4

// self test kbc returns
#define SELF_TEST_OK 0x55
#define SELF_TEST_ERROR 0xFC

// kbc command byte
#define DIS2 BIT(5) //mouse
#define DIS BIT(4) //keyboard
#define INT2 BIT(1) //mouse
#define INT BIT(0) //keyboard

// mouse byte commands (0xD4 args)
#define MOUSE_COMMAND_RESET 0xFF
#define MOUSE_COMMAND_RESEND 0xFE
#define MOUSE_COMMAND_SET_DEFAULTS 0xF6 
#define MOUSE_COMMAND_DISABLE_DATA_REPORTING 0xF5
#define MOUSE_COMMAND_ENABLE_DATA_REPORTING 0xF4
#define MOUSE_COMMAND_SET_SAMPLE_RATE 0xF3
#define MOUSE_COMMAND_SET_REMOTE_MODE 0xF0
#define MOUSE_COMMAND_SET_STREAM_MODE 0xEA
#define MOUSE_COMMAND_READ_DATA 0xEB
#define MOUSE_COMMAND_STATUS_REQUEST 0xE9
#define MOUSE_COMMAND_SET_RESOLUTION 0xE8
#define MOUSE_COMMAND_SET_SCALING21 0xE7 //acceleration mode
#define MOUSE_COMMAND_SET_SCALING11 0xE6 // linear mode

// mouse acknowledgment
#define MOUSE_ACK 0xFA // everything ok
#define MOUSE_NACK 0xFE // invalid byte; must restart command byte + arguments
#define MOUSE_ERROR 0xFC // second consecutive invalid byte; must restart command byte + arguments

// kbd key codes
#define ESC_BREAKCODE 0x81

// user defined functions
int (read_buffer_data)(bool aux_set);

void (kbd_ih)();
int (kbd_ph)();
void (mouse_ih)();
int (mouse_ph)();

int (kbd_subscribe_int)(uint8_t *bit_no);
int (kbd_unsubscribe_int)();
int (mouse_subscribe_int)(uint8_t *bit_no);
int (mouse_unsubscribe_int)();

void (kbc_read_command_byte)(uint8_t* command_byte);
int (kbc_write_command_byte)(uint8_t command_byte);
void (kbc_issue_command)(uint8_t command);

int (kbc_get_status)(uint8_t* status);

bool (may_write_command_byte)(uint8_t status_byte);
int (attempt_write_byte)();

void (delay_milli_seconds)(unsigned milli_seconds);
void (delay_micro_seconds)(unsigned micro_seconds);

// keyboard specific
bool (kbd_reenable_interruputs)();
bool (is_make_code)(uint8_t code);

// mouse specific
struct packet (generate_packet)(uint8_t* data);
int (kbc_mouse_disable_data_reporting)();
int (kbc_mouse_enable_data_reporting)();
int (kbc_write_mouse_byte)(uint8_t mouse_byte);

#endif /*_LCOM_KBC_H*/
