#include <stdlib.h>

// Constants
#define KBC_OUT_BUF 0x60
#define KBC_IN_BUF 0x60
#define KBC_STAT_REG 0x64

// Keyboard constants
#define IS_BREAK_CODE(code) ((code)&BIT(7))
#define KBD_IRQ 1

// Mouse constants
#define MOUSE_IRQ 12

// Status Register
#define KBC_STAT_PARITY BIT(7)
#define KBC_STAT_TIMEOUT BIT(6)
#define KBC_STAT_AUX BIT(5)
#define KBC_STAT_INH BIT(4)
#define KBC_STAT_A2 BIT(3)
#define KBC_STAT_SYS BIT(2)
#define KBC_STAT_IBF BIT(1)
#define KBC_STAT_OBF BIT(0)

// KBC Commands
#define KBD_CMD_WRITE_BYTE 0x60
#define KBD_CMD_READ_BYTE 0x20
#define KBD_DISABLE_MOUSE 0xA7
#define KBD_ENABLE_MOUSE 0xA8
#define KBD_CHECK_MOUSE 0xA9
#define KBD_WRITE_BYTE_MOUSE 0xD4

// Command byte
#define KBD_CB_INT_KBD BIT(0)
#define KBD_CB_DIS_KBD BIT(4)
#define KBD_CB_DIS2_MOUSE BIT(5)
#define KBD_CB_INT2_MOUSE BIT(1)

// Mouse commands
#define MOUSE_ENABLE_DATA_REPORTING 0xF4
#define MOUSE_DISABLE_DATA_REPORTING 0xF5
#define MOUSE_SET_REMOTE_MODE 0xF0
#define MOUSE_SET_STREAM_MODE 0xEA
#define MOUSE_READ_DATA 0xEB

// Mouse responses
#define MOUSE_ACK 0xFA
#define MOUSE_NACK 0xFE
#define MOUSE_ERROR 0xFC

// Key scancodes
#define ESC_BREAK_CODE 0x81
#define TWO_BYTE_CODE 0xE0

// Mouse bytes
#define MOUSE_LB BIT(0)
#define MOUSE_RB BIT(1)
#define MOUSE_MB BIT(2)
#define MOUSE_XOV BIT(6)
#define MOUSE_YOV BIT(7)
#define MOUSE_XSIGN BIT(4)
#define MOUSE_YSIGN BIT(5)

// Functions
int kbc_write_cmd(uint8_t cmd);
int kbc_read_obf(uint8_t *data);
