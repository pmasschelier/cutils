#ifndef CUTILS_BUMP_H
#define CUTILS_BUMP_H

#ifndef CUTILS_BUMP_SIZE_TYPE
#define CUTILS_BUMP_SIZE_TYPE unsigned
#include "allocator.h"
#endif

typedef CUTILS_BUMP_SIZE_TYPE bump_size_t;
typedef struct {
    char *memory;
    bump_size_t capacity;
    bump_size_t size;
} bump_allocator_t;

UNUSED
static void bump_reset(bump_allocator_t* bump) {
    bump->size = 0;
}

UNUSED
static bump_size_t bump_push_frame(const bump_allocator_t* bump) {
    return bump->size;
}

UNUSED
static void bump_pop_frame(bump_allocator_t* bump, const bump_size_t frame) {
    bump->size = frame;
}

#define BUMP_NEXT_ALLOC_ALIGNED(nextAlloc, align) ((nextAlloc) + ((align) - ((nextAlloc) % (align))))

UNUSED
static bump_allocator_t bump_init(void *memory, bump_size_t capacity) {
    return (bump_allocator_t){.memory = (char *)memory, .capacity = capacity, .size = 0};
}

UNUSED
static void bump_free(bump_allocator_t* bump) {
    *bump = (bump_allocator_t) {
        .memory = NULL,
        .capacity = 0,
        .size = 0
    };
}

UNUSED NODISCARD
static void *bump_allocate(bump_allocator_t *bump, const size_t size) {
    const bump_size_t nextAllocOffset = BUMP_NEXT_ALLOC_ALIGNED(bump->size, 64);
    const size_t minCapacity = nextAllocOffset + size;
    if(bump->capacity < minCapacity)
        return NULL;
    bump->size = nextAllocOffset + size;
    return bump->memory + nextAllocOffset;
}

UNUSED
static allocator_t bump_allocator(bump_allocator_t* bump) {
    return ALLOCATOR_INIT_METADATA(bump, (alloc_md_fn_t)bump_allocate, NULL, NULL);
}

#endif //CUTILS_BUMP_H