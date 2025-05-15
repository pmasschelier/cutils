#ifndef ARENA_H
#define ARENA_H

#include <cutils/when_macros.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct allocator allocator_t;

typedef void *(*alloc_fn_t)(allocator_t *, size_t);
typedef void (*dealloc_fn_t)(allocator_t *, void *);

struct allocator {
  alloc_fn_t alloc;
  dealloc_fn_t dealloc;
};

struct arena {
  allocator_t allocator;
  void *memory;
  size_t capacity;
  size_t size;
};

typedef struct arena arena_t;

extern size_t MINIMUM_ARENA_CAPACITY;

int arena_init(arena_t *arena);

void *arena_alloc(arena_t *arena, size_t size);

void arena_free(arena_t *arena);

// IMPLEMENTATION

size_t MINIMUM_ARENA_CAPACITY = 1024;

void dealloc_nop(allocator_t *, void *) {}

int arena_init(arena_t *arena) {
  arena->allocator.alloc = (alloc_fn_t)arena_alloc;
  arena->allocator.dealloc = dealloc_nop;
  arena->size = 0;
  arena->memory = NULL;
  arena->capacity = 0;
  return 0;
}

void *arena_alloc(arena_t *arena, size_t size) {
  void *ret;
  when_null_ret(arena, NULL);
  if (arena->size + size > arena->capacity) {
    size_t capacity = 2 * (arena->size + size);
    if (capacity < MINIMUM_ARENA_CAPACITY)
      capacity = MINIMUM_ARENA_CAPACITY;
    arena->memory = realloc(arena->memory, capacity);
    arena->capacity = capacity;
  }
  ret = (uint8_t *)arena->memory + arena->size;
  arena->size += size;
  return ret;
}

void arena_free(arena_t *arena) {
  free(arena->memory);
  arena->memory = NULL;
  arena->size = 0;
  arena->capacity = 0;
}

#endif // !ARENA_H
