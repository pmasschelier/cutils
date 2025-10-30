#ifndef CUTILS_BITS_H
#define CUTILS_BITS_H

#include <stdint.h>

#ifdef __GNUC__
UNUSED
static uint64_t bits_next_pow2(const uint64_t n) {
    return n == 1 ? 1 : 1 << (64 - __builtin_clzl(n - 1));
}
#else
UNUSED
static uint64_t bits_next_pow2(uint64_t n) {
    n |= (n >> 1);
    n |= (n >> 2);
    n |= (n >> 4);
    n |= (n >> 8);
    n |= (n >> 16);
    n |= (n >> 32);
    return n + 1;
}
#endif

#endif //CUTILS_BITS_H
