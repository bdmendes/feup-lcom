#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "kbc.h"
#include <stdbool.h>

#define KDB_IRQ 0x1

/* command byte */
#define INT BIT(0)

/* key codes */
#define ESC_BREAKCODE 0x81
#define FIRST_OF_TWO_BYTES 0xE0

/* functions */
bool (is_make_code)(uint8_t code);
void (kbd_ih)();
int (kbd_ph)();
int (kbd_subscribe_int)(uint8_t* bit_no);
int (kbd_unsubscribe_int)();
int (kbd_reenable_interrupts)();

#endif
