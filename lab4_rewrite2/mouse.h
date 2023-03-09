#include <lcom/lcf.h>

int mouse_subscribe_int(uint8_t *bit_no);

int mouse_unsubscribe_int();

int mouse_write_mouse_byte(uint8_t mouse_byte);

void mouse_read();

uint8_t* mouse_get_packet_byte();

uint8_t mouse_get_packet_index();

struct packet mouse_assemble_packet();