#include <tap.h>
#include <cutils/ring.h>

DEFINE_RING_TYPE(int)

int main(void) {
    plan(56);
    int_ring_t ring = ring_create_int(50, NULL);
    cmp_ok(ring.capacity, "==", 50, "Initial capacity is %d", 50);
    cmp_ok(ring.length, "==", 0, "Initial length is 0");
    ok(ring.data != NULL, "Initial data is allocated");

    for (unsigned i = 100; i < 200; i++) {
        int* a = ring_push_back_int(&ring, true);
        *a = i;
    }
    ring_foreach(ring, e, int){
        cmp_ok(*e, "==", 150 + index, "Index %d should be equal to (150 + %d)", index, index);
    }

    ring_free_int(&ring);
    cmp_ok(ring.capacity, "==", 0, "Set capacity = 0 on free");
    cmp_ok(ring.length, "==", 0, "Set length = 0 on free");
    ok(ring.data == NULL, "Set data = NULL on free");
    return 0;
}