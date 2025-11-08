#ifndef CUTILS_RING_H
#define CUTILS_RING_H

#include <cutils/alloc.h>
#include <cutils/when_macros.h>
#include <cutils/minmax.h>

#ifndef CUTILS_NO_STD
#include <string.h>
#endif

#define ring_empty(ring) ((ring).length == 0)
#define ring_full(ring) ((ring).length == (ring).capacity)
#define ring_next(ring, index) (((index) + 1) % (ring).capacity)
#define ring_prev(ring, index) (((index) + (ring).capacity - 1) % (ring).capacity)
#define ring_get_unsafe(ring, index) (&(ring).data[((ring).begin + (index)) % (ring).capacity])
#define ring_get(ring, index) ((index) < (ring).length ? ring_get_unsafe(ring, index) : NULL)
#define ring_init(type, buffer, size, free_element) (type ## _ring_t) { .data = (type*) buffer, .capacity = size, .free = free_element }
#define ring_foreach(ring, e, type)                                                     \
    for(type* e = ring_get(ring, 0); e != NULL; e = NULL)                               \
        for(unsigned index = 0; index != (ring).length && (e = ring_get_unsafe(ring, index)); index++)

#define DEFINE_RING_TYPE(type)              \
    DEFINE_INTERFACE_RING_TYPE(type)        \
    DEFINE_IMPLEMENTATION_RING_TYPE(type)   \

#define DEFINE_INTERFACE_RING_TYPE(type)                                        \
    typedef struct {                                                            \
        type* data;                                                             \
        void (*free)(type*);                                                    \
        unsigned capacity;                                                      \
        unsigned begin;                                                         \
        unsigned next;                                                          \
        unsigned length;                                                        \
    } type ## _ring_t;                                                          \

#define DEFINE_IMPLEMENTATION_RING_TYPE(type)                                   \
    UNUSED static type ## _ring_t ring_create_ ## type(                         \
        unsigned capacity, void (*free_element)(type*)                          \
    ) {                                                                         \
        when_false_ret(capacity > 0, (type ## _ring_t) { .capacity = 0 });      \
        return (type ## _ring_t) {                                              \
            .data = CUTILS_alloc(capacity * sizeof(type)),                      \
            .capacity = capacity,                                               \
            .free = free_element                                                \
        };                                                                      \
    }                                                                           \
                                                                                \
    UNUSED static void ring_free_ ## type(type ## _ring_t* ring) {              \
        if(ring->free) {                                                        \
            ring_foreach(*ring, e, type) {                                      \
                ring->free(e);                                                  \
            }                                                                   \
        }                                                                       \
        CUTILS_dealloc(ring->data);                                             \
        *ring = (type ## _ring_t) {                                             \
            .data = NULL,                                                       \
            .capacity = 0,                                                      \
            .begin = 0,                                                         \
            .next = 0,                                                          \
            .length = 0,                                                        \
        };                                                                      \
    }                                                                           \
                                                                                \
    UNUSED static type* ring_front_ ## type(type ## _ring_t* ring) {            \
        if(ring_empty(*ring))                                                   \
            return NULL;                                                        \
        return &ring->data[ring->begin];                                        \
    }                                                                           \
                                                                                \
    UNUSED static type* ring_back_ ## type(type ## _ring_t* ring) {             \
        if(ring_empty(*ring))                                                   \
            return NULL;                                                        \
        unsigned last = ring_prev(*ring, ring->next);                           \
        return &ring->data[last];                                               \
    }                                                                           \
                                                                                \
    UNUSED static unsigned ring_head_ ## type(                                  \
        type ## _ring_t* ring, unsigned count                                   \
    ) {                                                                         \
        if(ring_empty(*ring) || count >= ring->length) return ring->length;     \
        if (ring->free) {                                                       \
            for(unsigned index = count; index < ring->length; index++)          \
                ring->free(ring_get_unsafe(*ring, index));                      \
        }                                                                       \
        ring->length = count;                                                   \
        ring->next = (ring->begin + count) % ring->capacity;                    \
        return count;                                                           \
    }                                                                           \
                                                                                \
    UNUSED static unsigned ring_tail_ ## type(                                  \
        type ## _ring_t* ring, unsigned count                                   \
    ) {                                                                         \
        if(ring_empty(*ring) || count >= ring->length) return ring->length;     \
        if (ring->free && !ring_empty(*ring)) {                                 \
            for(unsigned index = 0; index < ring->length - count; index++)      \
                ring->free(ring_get_unsafe(*ring, index));                      \
        }                                                                       \
        ring->length = count;                                                   \
        ring->begin = (ring->capacity + ring->next - count) % ring->capacity;   \
        return count;                                                           \
    }                                                                           \
                                                                                \
    UNUSED static bool ring_pop_back_ ## type(                                  \
        type ## _ring_t* ring                                                   \
    ) {                                                                         \
        if(ring_empty(*ring))                                                   \
            return false;                                                       \
        unsigned last = ring_prev(*ring, ring->next);                           \
        if (ring->free)                                                         \
            ring->free(&ring->data[last]);                                      \
        ring->next = last;                                                      \
        ring->length -= 1;                                                      \
        return true;                                                            \
    }                                                                           \
                                                                                \
    UNUSED static bool ring_pop_front_ ## type(                                 \
        type ## _ring_t* ring                                                   \
    ) {                                                                         \
        if(ring_empty(*ring))                                                   \
            return false;                                                       \
        if (ring->free)                                                         \
            ring->free(&ring->data[ring->begin]);                               \
        ring->begin = ring_next(*ring, ring->begin);                            \
        ring->length -= 1;                                                      \
        return true;                                                            \
    }                                                                           \
                                                                                \
    UNUSED static type* ring_push_back_ ## type(                                \
        type ## _ring_t* ring, bool force                                       \
    ) {                                                                         \
        const unsigned next = ring_next(*ring, ring->next);                     \
        if(ring_full(*ring)) {                                                  \
            if (!force)                                                         \
                return NULL;                                                    \
            if (ring->free)                                                     \
                ring->free(&ring->data[ring->begin]);                           \
            ring->begin = ring_next(*ring, ring->begin);                        \
        } else {                                                                \
            ring->length += 1;                                                  \
        }                                                                       \
        const unsigned pop = ring->next;                                        \
        ring->next = next;                                                      \
        return &ring->data[pop];                                                \
    }                                                                           \

#endif //CUTILS_RING_H
