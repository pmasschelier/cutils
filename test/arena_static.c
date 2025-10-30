#include <string.h>
#include <tap.h>
#include <time.h>
#include <cutils/arena.h>

constexpr arena_size_t BUFFER_SIZE = 4321;
char buf[BUFFER_SIZE];

int main(void) {
    arena_t arena = arena_init(buf, BUFFER_SIZE);
    cmp_ok(arena.capacity, "==", BUFFER_SIZE, "Initial capacity");
    cmp_ok(arena.size, "==", 0, "Initial size");

    constexpr arena_size_t a_size = 5;
    unsigned char a_exp[a_size];
    memset(a_exp, 1, a_size);
    void* a = arena_allocate(&arena, a_size);
    memset(a, 1, a_size);

    constexpr arena_size_t b_size = 527;
    unsigned char b_exp[b_size];
    memset(b_exp, 2, b_size);
    void* b = arena_allocate(&arena, b_size);
    memset(b, 2, b_size);

    constexpr arena_size_t c_size = 97;
    unsigned char c_exp[c_size];
    memset(c_exp, 3, c_size);
    void* c = arena_allocate(&arena, c_size);
    memset(c, 3, c_size);

    cmp_mem(a, a_exp, a_size, "5 bytes zone check");
    cmp_mem(b, b_exp, b_size, "527 bytes zone check");
    cmp_mem(c, c_exp, c_size, "97 bytes zone check");

    const void* d = arena_allocate(&arena, 6000);
    ok(d == nullptr, "Return nullptr on overflow");

    arena_free(&arena);
    ok(arena.memory == nullptr, "Set memory = nullptr on free");
    cmp_ok(arena.capacity, "==", 0, "Set capacity = 0 on free");
    cmp_ok(arena.size, "==", 0, "Set size = 0 on free");
    return 0;
}