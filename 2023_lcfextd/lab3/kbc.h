#define KBD_IRQ 1
#define KBC_OUT_BUF 0x60
#define KBC_STAT_REG 0x64
#define KBC_IN_BUF_CMD 0x64
#define KBC_IN_BUF_ARG 0x60

#define KBD_ESC_KEY 0x81
#define KBD_FIRST_OF_TWO_BYTES 0xE0
#define KBD_KEY_IS_MAKE(byte) !(byte & BIT(7))

#define KBC_STAT_OBF BIT(0)
#define KBC_STAT_IBF BIT(1)
#define KBC_STAT_AUX BIT(5)
#define KBC_STAT_ERR (BIT(6) | BIT(7))

#define KBC_CMD_READ_BYTE 0x20
#define KBC_CMD_WRITE_BYTE 0x60

#define KBC_CMD_BYTE_DIS2 BIT(5)
#define KBC_CMD_BYTE_DIS BIT(4)
#define KBC_CMD_BYTE_INT2 BIT(1)
#define KBC_CMD_BYTE_INT BIT(0)

void enable_kbd_interrupts();