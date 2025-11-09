#ifndef CUTILS_ARRAY_H
#define CUTILS_ARRAY_H

#include <cutils/allocator/alloc.h>
#include <cutils/compatibility.h>
#include <cutils/minmax.h>
#include <stddef.h>
#include <assert.h>

#ifndef CUTILS_NO_STD
#include <string.h>
#endif

#ifndef ARRAY_MIN_CAPACITY
#define ARRAY_MIN_CAPACITY 64
#endif

#ifndef foreach
#define foreach(array, var, type)                                           \
    for(unsigned index = 0; index < (array).length; index = (array).length) \
    for(type* var = &(array).data[index];                                   \
        index < (array).length && (var = &(array).data[index]); index++)
#endif

#ifndef array_indexof
#define array_indexof(array, x) ((x) - (array).data)
#endif

#ifndef EMPTY_ARRAY
#define EMPTY_ARRAY { .length = 0, .capacity = 0, .data = NULL }
#endif


#define DEFINE_ARRAY_TYPE(type)                                                \
    DEFINE_INTERFACE_ARRAY_TYPE(type)                                          \
    DEFINE_IMPLEMENTATION_ARRAY_TYPE(type)                                     \

#define DEFINE_INTERFACE_ARRAY_TYPE(type)                                      \
    typedef struct {                                                           \
        unsigned length;                                                       \
        unsigned capacity;                                                     \
        type *data;                                                            \
    } type ## _array_t;

#define DEFINE_IMPLEMENTATION_ARRAY_TYPE(type)                                  \
    UNUSED NODISCARD static type *array_append_ ## type(                        \
        type ## _array_t *array, unsigned count                                 \
    ) {                                                                         \
        if(count == 0)                                                          \
            return NULL;                                                        \
        if (array->capacity == 0) {                                             \
            unsigned capacity = MAX(count, ARRAY_MIN_CAPACITY);                 \
            array->data = CUTILS_alloc(capacity * sizeof(type));                \
            array->capacity = capacity;                                         \
        }                                                                       \
        if (array->capacity < array->length + count) {                          \
            unsigned capacity = (array->capacity + count) * 2;                  \
            void *memory = CUTILS_realloc(array->data, capacity * sizeof(type));\
            assert(memory != NULL);                                             \
            array->data = memory;                                               \
            array->capacity *= 2;                                               \
        }                                                                       \
        type* ret = &array->data[array->length];                                \
        array->length += count;                                                 \
        return ret;                                                             \
    }                                                                           \
                                                                                \
    UNUSED static bool array_pop_ ## type(                                      \
        type ## _array_t *array, type* value                                    \
    ) {                                                                         \
        if(array->length == 0)                                                  \
            return false;                                                       \
        array->length -= 1;                                                     \
        if (value != NULL)                                                      \
            memcpy(value, &array->data[array->length], sizeof(type));           \
        return true;                                                            \
    }                                                                           \
                                                                                \
    UNUSED static type* array_insert_ ## type(                                  \
        type##_array_t *array, const type *item, unsigned index                 \
    ) {                                                                         \
        assert(index <= array->length);                                         \
        if(NULL == array_append_ ## type(array, 1))                             \
            return NULL;                                                        \
        size_t tailSize = (array->length - index) * sizeof(type);               \
        if(tailSize != 0)                                                       \
            memmove(&array->data[index + 1], &array->data[index], tailSize);    \
        if(item != NULL)                                                        \
            memcpy(&array->data[index], item, sizeof(type));                    \
        return &array->data[index];                                             \
    }                                                                           \
                                                                                \
    UNUSED static bool array_remove_ ## type(                                   \
        type##_array_t *array, const type *item, unsigned index                 \
    ) {                                                                         \
        assert(index < array->length);                                          \
        size_t tailSize = (array->length - 1 - index) * sizeof(type);           \
        if(item != NULL)                                                        \
            memcpy(&array->data[index], item, sizeof(type));                    \
        if(tailSize != 0)                                                       \
            memmove(&array->data[index], &array->data[index + 1], tailSize);    \
        array->length -= 1;                                                     \
        return true;                                                            \
    }                                                                           \
                                                                                \
    UNUSED static void array_filter_ ## type(                                   \
        type##_array_t *array, const bool* remove                               \
    ) {                                                                         \
        unsigned start = 0, blockSize = 0;                                      \
        for(unsigned i = 0; i < array->length; i++) {                           \
            if(remove[i]) {                                                     \
                if (blockSize > 0) {                                            \
                    memmove(&array->data[start], &array->data[i - blockSize],   \
                        blockSize * sizeof(type));                              \
                    start += blockSize;                                         \
                    blockSize = 0;                                              \
                }                                                               \
            } else {                                                            \
                blockSize++;                                                    \
            }                                                                   \
        }                                                                       \
    }                                                                           \
                                                                                \
    UNUSED static void array_swap_and_pop_back_ ## type(                        \
        type ## _array_t *array, unsigned index                                 \
    ) {                                                                         \
        assert(index < array->length);                                          \
        if (index == array->length - 1) {                                       \
            array->length--;                                                    \
            return;                                                             \
        }                                                                       \
        array->length--;                                                        \
        memcpy(array->data + index, array->data + array->length,                \
               sizeof(type));                                                   \
    }                                                                           \
                                                                                \
    UNUSED static void array_free_##type(type##_array_t *array) {               \
        free(array->data);                                                      \
        array->length = 0;                                                      \
        array->capacity = 0;                                                    \
        array->data = NULL;                                                     \
    }                                                                           \
                                                                                \
    UNUSED static type ## _array_t array_shallow_clone_ ## type(                \
        const type##_array_t *array                                             \
    ) {                                                                         \
        type* copy = malloc(array->capacity * sizeof(type));                    \
        memcpy(copy, array->data, array->length * sizeof(type));                \
        return (type ## _array_t) {                                             \
            .length = array->length,                                            \
            .capacity = array->capacity,                                        \
            .data = copy                                                        \
        };                                                                      \
    }                                                                           \
                                                                                \
    typedef int (*compare_ ## type ##_fn)(const type *a, const type *b);        \
                                                                                \
    UNUSED static type* array_insert_sorted_ ## type(                           \
        type ## _array_t* array, compare_ ## type ## _fn fn, type* x            \
    ) {                                                                         \
        if (array->length == 0 ||                                               \
            fn(x, &array->data[array->length - 1]) > 0) {                       \
            type *p = array_append_##type(array, 1);                            \
            memcpy(p, x, sizeof(type));                                         \
            return p;                                                           \
        } else if (fn(x, &array->data[0]) < 0) {                                \
            return array_insert_##type(array, x, 0);                            \
        }                                                                       \
        unsigned a = 0, b = array->length, mid;                                 \
        while (a <= b) {                                                        \
            mid = (a + b) / 2;                                                  \
            int cmp = fn(x, &array->data[mid]);                                 \
            if (cmp < 0) {                                                      \
                b = mid - 1;                                                    \
            } else if (cmp > 0) {                                               \
                a = mid + 1;                                                    \
            } else {                                                            \
                return array_insert_##type(array, x, mid);                      \
            }                                                                   \
        }                                                                       \
        return array_insert_##type(array, x, mid);                              \
    }                                                                           \
                                                                                \
    UNUSED static type* array_find_sorted_ ## type(                             \
        const type ## _array_t* array, compare_ ## type ## _fn fn, type* x      \
    ) {                                                                         \
        if (array->length == 0                                                  \
            || fn(x, &array->data[0]) < 0 ||                                    \
            fn(x, &array->data[array->length - 1]) > 0) {                       \
            return NULL;                                                        \
        }                                                                       \
        unsigned a = 0, b = array->length, mid;                                 \
        while (a <= b) {                                                        \
            mid = (a + b) / 2;                                                  \
            int cmp = fn(x, &array->data[mid]);                                 \
            if (cmp < 0) {                                                      \
                b = mid - 1;                                                    \
            } else if (cmp > 0) {                                               \
                a = mid + 1;                                                    \
            } else {                                                            \
                return &array->data[mid];                                       \
            }                                                                   \
        }                                                                       \
        return NULL;                                                            \
    }

#endif //CUTILS_ARRAY_H
