#pragma once

#include "stdbool.h"
#include <stdint.h>

typedef uint8_t bitmask;
typedef uint16_t wide_bitmask;
typedef int bitpos;

#define MSK_END -1

/**
 * Builds a bitmask with bits set at given positions. The variable argument list ends at the sentinel `MSK_END` value, by convention.
 */
bitmask mask(bitpos pos, ...);

bitmask clear(bitmask msk, bitpos pos);

bitmask set(bitmask msk, bitpos pos);

bool is_set(bitmask msk, bitpos pos);

bitmask lsb(wide_bitmask wide_msk);

bitmask msb(wide_bitmask wide_msk);
