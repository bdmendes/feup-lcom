#include <stdlib.h>

#define IS_BREAK_CODE(code) ((code)&BIT(7))

#define KBD_IRQ 1
#define KBD_OUT_BUF 0x60
#define KBD_IN_BUF 0x60
#define KBD_STAT_REG 0x64

// Status Register
#define KBD_STAT_PARITY BIT(7)
#define KBD_STAT_TIMEOUT BIT(6)
#define KBD_STAT_AUX BIT(5)
#define KBD_STAT_INH BIT(4)
#define KBD_STAT_A2 BIT(3)
#define KBD_STAT_SYS BIT(2)
#define KBD_STAT_IBF BIT(1)
#define KBD_STAT_OBF BIT(0)

// KBC Commands
#define KBD_CMD_WRITE_BYTE 0x60
#define KBD_CMD_READ_BYTE 0x20
#define KBD_CMD_CHECK_KBD 0xAB
#define KBD_CMD_SELF_TEST 0xAA
#define KBD_CMD_DISABLE 0xAD
#define KBD_CMD_ENABLE 0xAE

// Command byte
#define KBD_CB_INT BIT(0)
#define KBD_CB_DIS BIT(4)
#define KBD_CB_DIS2 BIT(5)
#define KBD_CB_INT2 BIT(1)

// Scancodes
#define ESC_BREAK_CODE 0x81
#define TWO_BYTE_CODE 0xE0

// Functions
uint8_t *kbc_get_out_buf();
int kbc_subscribe_int(uint8_t *bit_no);
int kbc_unsubscibe_int();
int kbc_read_byte();
int kbc_disable_int();
int kbc_enable_int();
void kbc_interrupt_handler();
