#include "bitwise.h"
#include "stdarg.h"

bitmask mask(bitpos pos, ...) {
    bitmask res = 0;
    bitpos bitpos;
    va_list list;

    va_start(list, pos);
    res |= 1 << pos;

    while (1) {
        bitpos = va_arg(list, int);
        if (bitpos == MSK_END) break;
        res |= 1 << bitpos;
    }
    va_end(list);
    return res;
}

bitmask clear(bitmask msk, bitpos pos) {
    return msk & ~(1 << pos);
}

bitmask set(bitmask msk, bitpos pos) {
    return msk | (1 << pos);
}

bool is_set(bitmask msk, bitpos pos) {
    return msk & (1 << pos);
}

bitmask lsb(wide_bitmask wide_msk) {
    return wide_msk;
}

bitmask msb(wide_bitmask wide_msk) {
    return wide_msk >> 8;
}
