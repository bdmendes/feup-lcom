#ifndef LAB4_UTILS_H
#define LAB4_UTILS_H

#include <lcom/lcf.h>

// functions
int(util_sys_inb)(int port, uint8_t * value);
void (delay_milli_seconds)(unsigned milli_seconds);
void (delay_micro_seconds)(unsigned micro_seconds);

#endif // LAB4_UTILS_H
