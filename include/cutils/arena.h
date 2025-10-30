#ifndef CUTILS_ARENA_H
#define CUTILS_ARENA_H

#include <cutils/when_macros.h>
#include <cutils/allocator.h>
#include <stddef.h>

#ifndef CUTILS_ARENA_SIZE_TYPE
#define CUTILS_ARENA_SIZE_TYPE unsigned
#endif

#ifdef CUTILS_ARENA_DYNAMIC
#if !defined(CUTILS_ARENA_realloc) || !defined(CUTILS_ARENA_malloc)
#include <stdlib.h>
#endif
#ifndef CUTILS_ARENA_malloc
#define CUTILS_ARENA_malloc malloc
#endif
#ifndef CUTILS_ARENA_realloc
#define CUTILS_ARENA_realloc realloc
#endif
#endif

typedef CUTILS_ARENA_SIZE_TYPE arena_size_t;

typedef struct arena arena_t;

struct arena {
  char *memory;
  arena_size_t capacity;
  arena_size_t size;
};

extern size_t MINIMUM_ARENA_CAPACITY;

[[maybe_unused]]
static void arena_reset(arena_t* arena) {
  arena->size = 0;
}

[[maybe_unused]]
static arena_size_t arena_push_frame(const arena_t* arena) {
  return arena->size;
}

[[maybe_unused]]
static void arena_pop_frame(arena_t* arena, const arena_size_t frame) {
  arena->size = frame;
}

#define ARENA_NEXT_ALLOC_ALIGNED(nextAlloc, align) ((nextAlloc) + ((align) - ((nextAlloc) % (align))))

#ifdef CUTILS_ARENA_DYNAMIC
[[maybe_unused]]
static arena_t arena_init(arena_size_t capacity) {
  void* memory = CUTILS_ARENA_malloc(capacity);
  if (memory == nullptr) capacity = 0;
  return (arena_t){.memory = memory, .capacity = capacity, .size = 0};
}

[[maybe_unused]]
static void arena_free(arena_t* arena) {
  free(arena->memory);
  *arena = (arena_t) {
    .memory = nullptr,
    .capacity = 0,
    .size = 0
    };
}

[[maybe_unused]] [[nodiscard]]
static void *arena_allocate(arena_t *arena, const size_t size) {
  const arena_size_t nextAllocOffset = ARENA_NEXT_ALLOC_ALIGNED(arena->size, 64);
  const size_t minCapacity = nextAllocOffset + size;
  if (arena->capacity < minCapacity) {
    void* memory = CUTILS_ARENA_realloc(arena->memory, 2 * minCapacity);
    when_null_ret(memory, nullptr);
    if (memory == nullptr) return nullptr;
    arena->memory = memory;
    arena->capacity = 2 * minCapacity;
  }
  arena->size = minCapacity;
  return arena->memory + nextAllocOffset;
}
#else
[[maybe_unused]]
static arena_t arena_init(void *memory, arena_size_t capacity) {
  return (arena_t){.memory = (char *)memory, .capacity = capacity, .size = 0};
}

[[maybe_unused]]
static void arena_free(arena_t* arena) {
  *arena = (arena_t) {
    .memory = nullptr,
    .capacity = 0,
    .size = 0
    };
}

[[maybe_unused]] [[nodiscard]]
static void *arena_allocate(arena_t *arena, size_t size) {
  const arena_size_t nextAllocOffset = ARENA_NEXT_ALLOC_ALIGNED(arena->size, 64);
  const size_t minCapacity = nextAllocOffset + size;
  if(arena->capacity < minCapacity)
    return nullptr;
  arena->size = nextAllocOffset + size;
  return arena->memory + nextAllocOffset;
}
#endif

[[maybe_unused]]
static allocator_t arena_allocator(arena_t* arena) {
  return ALLOCATOR_INIT_METADATA(arena, (alloc_md_fn_t)arena_allocate, nullptr, nullptr);
}

#endif // !CUTILS_ARENA_H
