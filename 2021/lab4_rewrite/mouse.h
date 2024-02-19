#ifndef MOUSE_H
#define MOUSE_H

#include <lcom/lcf.h>
#include "kbc.h"

#define MOUSE_IRQ 12

/* Mouse related KBC commands */
#define KBC_DISABLE_MOUSE 0xA7
#define KBC_ENABLE_MOUSE 0xA8
#define KBC_CHECK_MOUSE_INTERFACE 0xA9
#define KBC_WRITE_BYTE_TO_MOUSE 0xD4

/* Mouse commands */
#define MOUSE_RESET 0xFF
#define MOUSE_RESEND 0xFE
#define MOUSE_SET_DEFAULTS 0xF6
#define MOUSE_DISABLE_DATA_REPORTING 0xF5
#define MOUSE_ENABLE_DATA_REPORTING 0xF4
#define MOUSE_SET_SAMPLE_RATE 0xF3
#define MOUSE_SET_REMOTE_MODE 0xF0
#define MOUSE_READ_DATA 0xEB
#define MOUSE_SET_STREAM_MODE 0xEA
#define MOUSE_STATUS_REQUEST 0xE9
#define MOUSE_SET_RESOLUTION 0xE8
#define MOUSE_SET_ACCELERATION_MODE 0xE7
#define MOUSE_SET_LINEAR_MODE 0xE6

/* Mouse packet parsing */
#define MOUSE_PCKT_FIRST_BYTE_FLAG BIT(3)
#define MOUSE_BYTE_1_Y_OVFL BIT(7)
#define MOUSE_BYTE_1_X_OVFL BIT(6)
#define MOUSE_BYTE_1_Y_DELTA_MSB BIT(5)
#define MOUSE_BYTE_1_X_DELTA_MSB BIT(4)
#define MOUSE_BYTE_1_MB BIT(2)
#define MOUSE_BYTE_1_RB BIT(1)
#define MOUSE_BYTE_1_LB BIT(0)

/* Mouse ACK bytes */
#define MOUSE_ACK 0xFA
#define MOUSE_NACK 0xFE
#define MOUSE_ERROR 0xFC




/* Mouse functions */
int(mouse_subscribe_int)(uint8_t *bit_no);
int(mouse_unsubscribe_int)();
void(mouse_ih)();
int(mouse_write_command)(uint8_t mouse_cmd_byte);
void(mouse_assemble_packet)(uint8_t* data, struct packet *pp);
int(mouse_disable_data_reporting)();
int(m_mouse_enable_data_reporting)();

#endif // MOUSE_H
