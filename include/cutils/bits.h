#ifndef CUTILS_BITS_H
#define CUTILS_BITS_H

#include <stdint.h>
#include <cutils/compatibility.h>

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

UNUSED
static bool bits_isset(const uint8_t* bits, const unsigned index) {
    const uint8_t byte = bits[index / 8];
    return 1 & (byte >> (index % 8));
}

UNUSED
static void bits_set(uint8_t* bits, const unsigned index) {
    const uint8_t byte = bits[index >> 3];
    const uint8_t mask = (1 << (index & 7));
    bits[index >> 3] = byte | mask;
}

UNUSED
static void bits_reset(uint8_t* bits, const unsigned index) {
    const uint8_t byte = bits[index >> 3];
    const uint8_t mask = (1 << (index & 7));
    bits[index >> 3] = byte & (~mask);
}

UNUSED
static bool bits_any(const uint8_t* bits, const unsigned from, const unsigned to) {
    const uint8_t start = bits[from / 8] >> (from % 8);
    if((from >> 3) == (to >> 3))
        return (start << (8 - ((to - from) % 8))) != 0;
    if(start != 0)
        return true;
    for(unsigned i = (from >> 3) + 1; i < (to >> 3) - 1; i++)
        if(bits[i]) return true;
    const uint8_t end = bits[to / 8] << (7 - (to % 8));
    return end != 0;
}

#endif //CUTILS_BITS_H
