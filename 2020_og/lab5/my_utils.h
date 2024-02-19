#ifndef MY_UTILS
#define MY_UTILS

#include <lcom/lcf.h>

void (delay_milli_seconds)(unsigned milli_seconds);
void (delay_micro_seconds)(unsigned micro_seconds);
void (delay_seconds)(unsigned seconds);
uint32_t bit_range(uint8_t start, uint8_t end);

#endif
