#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "kbc.h"
#include <stdbool.h>

#define KDB_IRQ 0x1

/* command byte */
#define INT BIT(0)

/* add from here: https://www.win.tue.nl/~aeb/linux/kbd/scancodes-1.html */
/* key makecodes */
#define KEYBOARD_FAIL_MAKECODE 0x0
#define ESC_MAKECODE 0x01
#define ENTER_MAKECODE 0x1c
#define SPACE_BAR_MAKECODE 0x39
#define W_MAKECODE 0x11
#define A_MAKECODE 0x1e
#define S_MAKECODE 0x1f
#define D_MAKECODE 0x20
#define UP_ARROW_MAKECODE 0x48
#define DOWN_ARROW_MAKECODE 0x50
#define LEFT_ARROW_MAKECODE 0x4b
#define RIGHT_ARROW_MAKECODE 0x4d

/* misc */
#define FIRST_OF_TWO_BYTES 0xE0

/* functions */
/**
 * @brief  checks if a scancode is a make code
 * @param  code: the scancode
 * @retval true if it's a scancode, false otherwise
 */
bool(is_make_code)(uint8_t code);

/**
 * @brief  gets the breakcode corresponding to a given makecode
 * @param  make_code: the makecode
 * @retval the corresponding breakcode
 */
uint8_t(get_break_code)(uint8_t make_code);

/**
 * @brief  the keyboard interrupt handler: calls get_buffer_data()
 * @retval None
 */
void(kbd_ih)();

/**
 * @brief  subscribes keyboard interruptions
 * @param  bit_no: the bit number
 * @retval OK if success, !OK otherwise
 */
int(kbd_subscribe_int)(uint8_t *bit_no);

/**
 * @brief  unsibscribes keyboard interrupts
 * @retval OK if success, !OK otherwise
 */
int(kbd_unsubscribe_int)();

/**
 * @brief  reenables keyboard interrupts
 * @retval OK if success, !OK otherwise
 */
int(kbd_reenable_interrupts)();

#endif
