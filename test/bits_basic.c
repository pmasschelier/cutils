#include <cutils/bits.h>
#include <tap.h>

uint8_t bitset[512];

int main(void) {
    CONSTEXPR size_t length = (512 / 2) * 8 - 15;
    for (unsigned i = 0; i < 512 * 8; i++) {
        if (i % 7 == 0 && i < length) bits_set(bitset, i);
        else bits_reset(bitset, i);
    }
    for (unsigned i = 0; i < 512 * 8; i++) {
        cmp_ok(bits_isset(bitset, i), "==", i % 7 == 0 && i < length);
    }
    ok(bits_any(bitset, 15, 20) == false, "Only multiples of seven are set");
    ok(bits_any(bitset, 15, 21) == true, "Multiples of seven are set");
    return 0;
}