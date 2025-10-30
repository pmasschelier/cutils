#ifndef CUTILS_ALLOCATOR_H
#define CUTILS_ALLOCATOR_H

#include <stddef.h>

typedef void *(*alloc_fn_t)(size_t size);
typedef void *(*realloc_fn_t)(void* buffer, size_t size);
typedef void (*dealloc_fn_t)(void* buffer);

typedef void *(*alloc_md_fn_t)(void* metadata, size_t size);
typedef void *(*realloc_md_fn_t)(void* metadata, void* buffer, size_t size);
typedef void (*dealloc_md_fn_t)(void* metadata, void* buffer);

typedef struct allocator allocator_t;
struct allocator {
    void* metadata;
    union {
        struct {
            alloc_fn_t alloc;
            realloc_fn_t realloc;
            dealloc_fn_t dealloc;
        } no_md;
        struct {
            alloc_md_fn_t alloc;
            realloc_md_fn_t realloc;
            dealloc_md_fn_t dealloc;
        } md;
    };
};

#define GET_MACRO(_1, _2, _3, NAME, ...) NAME
#define ALLOCATOR_INIT_METADATA(_metadata, _alloc, _realloc, _dealloc) (allocator_t) { .metadata = _metadata, .md = { .alloc = _alloc, .realloc = _realloc, .dealloc = _dealloc } }
#define ALLOCATOR_INIT_NO_METADATA(_alloc, _realloc, _dealloc) (allocator_t) { .metadata = nullptr, .no_md = { .alloc = _alloc, .realloc = _realloc, .dealloc = _dealloc } }
#define ALLOCATOR_INIT(...) GET_MACRO(__VA_ARGS__, ALLOCATOR_INIT_NO_METADATA, ALLOCATOR_INIT_METADATA)(__VA_ARGS__)

#ifndef CUTILS_NO_STD
#include <cutils/alloc.h>
#define ALLOCATOR_DEFAULT ALLOCATOR_INIT_NO_METADATA(CUTILS_alloc, CUTILS_realloc, CUTILS_dealloc)
#endif

[[maybe_unused]]
static void* AllocatorAlloc(const allocator_t allocator, const size_t size) {
    if (allocator.metadata)
        return allocator.md.alloc(allocator.metadata, size);
    return allocator.no_md.alloc(size);
}

[[maybe_unused]]
static void* AllocatorRealloc(const allocator_t allocator, void* buffer, const size_t size) {
    if (allocator.metadata)
        return allocator.md.realloc(allocator.metadata, buffer, size);
    return allocator.no_md.realloc(buffer, size);
}
[[maybe_unused]]
[[maybe_unused]]
static void AllocatorDealloc(const allocator_t allocator, void* buffer) {
    if (allocator.metadata)
        allocator.md.dealloc(allocator.metadata, buffer);
    else allocator.no_md.dealloc(buffer);
}

#endif //CUTILS_ALLOCATOR_H