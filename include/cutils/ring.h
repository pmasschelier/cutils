#ifndef CUTILS_RING_H
#define CUTILS_RING_H

#include <cutils/alloc.h>
#include <cutils/when_macros.h>

#ifndef CUTILS_NO_STD
#include <string.h>
#endif

#define ring_empty(ring) ((ring).begin == (ring).next)

#define ring_next(ring, index) (((index) + 1) % (ring).capacity)
#define ring_prev(ring, index) (((index) - 1) % (ring).capacity)

#define DEFINE_RING_TYPE(type)                                                  \
    typedef struct {                                                            \
        type* data;                                                             \
        unsigned capacity;                                                      \
        unsigned begin;                                                         \
        unsigned next;                                                          \
        bool full;                                                              \
    } type ## _ring_t;                                                          \
                                                                                \
    [[maybe_unused]] static type ## _ring_t ring_init_ ## type(                 \
        unsigned capacity                                                       \
    ) {                                                                         \
        assert(capacity > 0);                                                   \
        return (type ## _ring_t) {                                              \
            .data = CUTILS_alloc(capacity * sizeof(type)),                      \
            .capacity = capacity,                                               \
        };                                                                      \
    }                                                                           \
                                                                                \
    [[maybe_unused]] static void ring_deinit_ ## type(type ## _ring_t* ring) {  \
        CUTILS_dealloc(ring->data);                                             \
        ring->capacity = 0;                                                     \
        ring->begin = 0;                                                        \
        ring->next = 0;                                                         \
        ring->full = false;                                                     \
    }                                                                           \
                                                                                \
    [[maybe_unused]] static type* ring_front_ ## type(type ## _ring_t* ring) {  \
        if(ring_empty(*ring))                                                   \
            return nullptr;                                                     \
        return &ring->data[ring->begin];                                        \
    }                                                                           \
                                                                                \
    [[maybe_unused]] static bool ring_pop_back_ ## type(                        \
        type ## _ring_t* ring, type* popped                                     \
    ) {                                                                         \
        if(ring_empty(*ring))                                                   \
            return false;                                                       \
        unsigned last = ring_prev(*ring, ring->next);                           \
        if(popped != nullptr)                                                   \
            memcpy(popped, &ring->data[last], sizeof(type));                    \
        ring->next = last;                                                      \
        ring->full = false;                                                     \
        return true;                                                            \
    }                                                                           \
                                                                                \
    [[maybe_unused]] static bool ring_pop_front_ ## type(                       \
        type ## _ring_t* ring, type* popped                                     \
    ) {                                                                         \
        if(ring_empty(*ring))                                                   \
            return false;                                                       \
        if(popped != nullptr)                                                   \
            memcpy(popped, &ring->data[ring->begin], sizeof(type));             \
        ring->begin = ring_next(*ring, ring->begin);                            \
        ring->full = false;                                                     \
        return true;                                                            \
    }                                                                           \
                                                                                \
    [[maybe_unused]] static type* ring_push_back_ ## type(                      \
        type ## _ring_t* ring, bool force                                       \
    ) {                                                                         \
        const unsigned next = ring_next(*ring, ring->next);                     \
        if(ring->full) {                                                        \
            if (!force)                                                         \
                return nullptr;                                                 \
            ring->begin = ring_next(*ring, ring->begin);                        \
        } else if(next == ring->begin) {                                        \
            ring->full = true;                                                  \
        }                                                                       \
        const unsigned pop = ring->next;                                        \
        ring->next = next;                                                      \
        return &ring->data[pop];                                                \
    }                                                                           \

#endif //CUTILS_RING_H
