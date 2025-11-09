#include <string.h>
#include <tap.h>
#include <time.h>
#include <../include/cutils/allocator/bump.h>

#define BUFFER_SIZE 4321
char buf[BUFFER_SIZE];

int main(void) {
    bump_allocator_t bump = bump_init(buf, BUFFER_SIZE);

    CONSTEXPR bump_size_t a_size = 5;
    unsigned char a_exp[a_size];
    memset(a_exp, 1, a_size);
    void* a = bump_allocate(&bump, a_size);
    memset(a, 1, a_size);

    CONSTEXPR bump_size_t b_size = 527;
    unsigned char b_exp[b_size];
    memset(b_exp, 2, b_size);
    void* b = bump_allocate(&bump, b_size);
    memset(b, 2, b_size);

    CONSTEXPR bump_size_t c_size = 97;
    unsigned char c_exp[c_size];
    memset(c_exp, 3, c_size);
    void* c = bump_allocate(&bump, c_size);
    memset(c, 3, c_size);

    cmp_mem(a, a_exp, a_size, "5 bytes zone check");
    cmp_mem(b, b_exp, b_size, "527 bytes zone check");
    cmp_mem(c, c_exp, c_size, "97 bytes zone check");

    const void* d = bump_allocate(&bump, 6000);
    ok(d == NULL, "Return NULL on overflow");

    bump_free(&bump);
    ok(bump.memory == NULL, "Set memory = nullptr on free");
    cmp_ok(bump.capacity, "==", 0, "Set capacity = 0 on free");
    cmp_ok(bump.size, "==", 0, "Set size = 0 on free");
    return 0;
}