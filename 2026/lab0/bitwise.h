#pragma once

#include "stdbool.h"
#include <stdint.h>

typedef uint8_t bitmask;
typedef uint16_t wide_bitmask;
typedef int bitpos;

#define MSK_END -1

/**
 * Builds a bitmask with bits set at given positions. The variable argument list
 * ends at the sentinel `MSK_END` value, by convention.
 */
bitmask mask(bitpos pos, ...);

/**
 * Yields a new bitmask equal to `msk`.
 */
bitmask clear(bitmask msk, bitpos pos);

/**
 * Yields a new bitmask equal to `msk`  with bit at `pos` set to 1.
 */
bitmask set(bitmask msk, bitpos pos);

/**
 * Returns true if bit at `pos` in `msk` is set to 1, false otherwise.
 */
bool is_set(bitmask msk, bitpos pos);

/**
 * Returns the least significant byte of a 16-bit wide bitmask.
 */
bitmask lsb(wide_bitmask wide_msk);

/**
 * Returns the most significant byte of a 16-bit wide bitmask.
 */
bitmask msb(wide_bitmask wide_msk);
