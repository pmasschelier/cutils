#ifndef CUTILS_ARENA_H
#define CUTILS_ARENA_H

#include <cutils/when_macros.h>
#include <cutils/allocator.h>
#include <cutils/bits.h>

#ifndef MIN
#define MIN(a,b) (a) < (b) ? (a) : (b)
#endif

#ifndef MAX
#define MAX(a,b) (a) > (b) ? (a) : (b)
#endif

#ifndef CUTILS_ARENA_SIZE_TYPE
#define CUTILS_ARENA_SIZE_TYPE size_t
#endif

#ifndef CUTILS_ARENA_DEFAULT_REGION_SIZE
#define CUTILS_ARENA_DEFAULT_REGION_SIZE 4096
#endif

typedef CUTILS_ARENA_SIZE_TYPE arena_size_t;

typedef struct arena_region arena_region_t;

struct arena_region {
  arena_region_t* next;
  arena_size_t capacity;
  arena_size_t used;
  char data[];
};

#define CUTILS_NEXT_ALLOC_ALIGNED(nextAlloc, align) (((nextAlloc) + ((align) - 1)) & -align)
#define CUTILS_SIZE

UNUSED NODISCARD
static arena_region_t* arena_region_allocate(const arena_size_t capacity) {
  const arena_size_t size = CUTILS_NEXT_ALLOC_ALIGNED(sizeof(arena_region_t) + capacity, 64);
  arena_region_t* region = CUTILS_alloc(size);
  when_null_ret(region, NULL);
  region->next = NULL;
  region->capacity = capacity;
  region->used = 0;
  return region;
}

typedef struct arena_allocator arena_allocator_t;

struct arena_allocator {
  arena_region_t* head;
  arena_region_t *current;
};

#if __STDC_VERSION__ >= 202311L
#define ARENA_INIT {}
#else
#define ARENA_INIT { NULL, NULL }
#endif

NODISCARD
static void *arena_allocate(arena_allocator_t *arena, const arena_size_t size) {
  // Compute the alignment
  const arena_size_t align = MIN(bits_next_pow2(size), 64);
  // Minimum region size to allocate if needed
  const arena_size_t regionSize = MAX(size, CUTILS_ARENA_DEFAULT_REGION_SIZE);
  // Minimum region size needed
  arena_size_t needed;
  if (arena->current == NULL) {
    needed = size;
    arena->head = arena->current = arena_region_allocate(regionSize);
  } else {
    while ((needed = CUTILS_NEXT_ALLOC_ALIGNED(arena->current->used, align) + size) > arena->current->capacity
      && arena->current->next != NULL)
      arena->current = arena->current->next;
  }
  // If the current block is not large enough allocate another
  if (needed > arena->current->capacity) {
    arena->current->next = arena_region_allocate(regionSize);
    arena->current = arena->current->next;
  }
  void* ret = &arena->current->data[arena->current->used];
  arena->current->used += needed;
  return ret;
}

UNUSED NODISCARD
static void *arena_allocate_array(arena_allocator_t *arena, const arena_size_t count, const arena_size_t size) {
  return arena_allocate(arena, count * size);
}

UNUSED
static void arena_reset(arena_allocator_t* arena) {
  for (arena_region_t* r = arena->head; r != NULL; r = r->next)
    r->used = 0;
  arena->current = arena->head;
}

UNUSED
static void arena_free(arena_allocator_t* arena) {
  arena_region_t* r = arena->head;
  while (r != NULL) {
    arena_region_t* remove = r;
    r = r->next;
    CUTILS_dealloc(remove);
  }

  arena->head = NULL;
  arena->current = NULL;
}

static void arena_free_noop(arena_allocator_t*, void*) {}

UNUSED
static allocator_t arena_get_allocator(arena_allocator_t* arena) {
  return ALLOCATOR_INIT_METADATA(arena, (alloc_md_fn_t)arena_allocate, NULL, (dealloc_md_fn_t)arena_free_noop);
}

typedef struct {
  arena_region_t* region;
  unsigned used;
} arena_frame_t;

UNUSED
static arena_frame_t arena_push_frame(const arena_allocator_t* arena) {
  return (arena_frame_t) { arena->current, arena->current->used };
}

UNUSED
static void arena_pop_frame(arena_allocator_t* arena, const arena_frame_t frame) {
  arena->current = frame.region;
  arena->current->used = frame.used;
}

#endif // !CUTILS_ARENA_H
