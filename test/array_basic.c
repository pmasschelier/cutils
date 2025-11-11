#include <tap.h>
#include <cutils/array.h>

DEFINE_ARRAY_TYPE(int)

int main(void) {
    int_array_t array = EMPTY_ARRAY(int);
    cmp_ok(array.capacity, "==", 0, "Initial capacity is 0");
    cmp_ok(array.length, "==", 0, "Initial length is 0");
    ok(array.data == NULL, "Initial data is NULL");



    array_free_int(&array);
    cmp_ok(array.capacity, "==", 0, "Set capacity = 0 on free");
    cmp_ok(array.length, "==", 0, "Set length = 0 on free");
    ok(array.data == NULL, "Set data = NULL on free");
    return 0;
}