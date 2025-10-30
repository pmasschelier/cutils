#ifndef ARRAY_LIST_H
#define ARRAY_LIST_H

/**
 * @file array_list.h
 * @brief Dynamic arrays definition
 * Defines functions to create, free and manipulate dynamic arrays
 * @ingroup array_list
 */

#include "cutils/errors.h"
#include "cutils/when_macros.h"

#define ARRAY_LIST_MIN_CAPACITY 8

#define get_array_list_ref(array, index, type)                                 \
  (type *)((array).data + (index) * (array).size_bytes)

/**
 * @defgroup array_list Dynamic arrays
 * @{
 */

/**
 * @typedef array_list_t
 * @brief Typedef for the array_list structure
 *
 */
typedef struct array_list array_list_t;

/**
 * @struct circular_buffer
 * @brief A circular buffer
 *
 * A dynamic array olds a pointer to its data, the size (in bytes) of an
 * element of the array, the current capacity of the buffer, the actual
 * number of elements in the array
 */
struct array_list {
  char *data; /**< Pointer to the allocated memory for the dynamic array */
  unsigned size_bytes; /**< Size (in bytes) of an element of the array */
  unsigned capacity;   /**< The current capacity of the array */
  unsigned size;       /**< The number of elements in the array */
};

#define ARRAY_LIST_INIT(type) (array_list_t){NULL, sizeof(type), 0, 0};

/**
 * @brief Create an empty dynamic array
 *
 * __Every array_list created with this function should be freed using
 * array_list_free()_
 *
 * _Complexity: O(1)_
 * @param[in] size Size of an element
 * @return A pointer to the newly created array_list
 * @see array_list_free()
 */
array_list_t *array_list_create(unsigned size_bytes);

/**
 * @brief Free the array
 *
 * _Complexity: O(1)_
 * @param[in] array pointer to the array
 */
void array_list_free(array_list_t *array);

/**
 * @brief Initializes a preallocated dynamic array
 *
 * __Every array_list created with this function should be freed using
 * array_list_deinit()_
 *
 * _Complexity: O(1)_
 * @param[in] size Size of an element
 * @return A pointer to the newly created array_list
 * @see array_list_deinit()
 */
void array_list_init(array_list_t *array, unsigned size_bytes);

/**
 * @brief Free an array initialized with array_list_init()
 *
 * _Complexity: O(1)_
 * @param[in] array pointer to the array
 */
void array_list_deinit(array_list_t *array);

/**
 * @brief Test if the array is empty
 *
 * What it really does : `return array->size == 0;`
 *
 * _Complexity: O(1)_
 * @param[in] array pointer on the array to test
 * @return TRUE iif the array is empty
 */
bool array_list_empty(array_list_t *array);

/**
 * @brief Copy an element at the beginning of the array
 *
 * The array **won't** take ownership of the element but extend the array buffer
 * and copy the element into it.
 *
 * _Complexity: O(n)_
 * @param array Pointer to the array to extend
 * @param p Pointer to the data to copy at the beginning of the array
 * @return Pointer to the newly created element (NULL in case of failure)
 */
void *array_list_push_front(array_list_t *array, void *value);

/**
 * @brief Copy an element at the end of the array
 *
 * The array **won't** take ownership of the element but extend the array buffer
 * and copy the element into it.
 *
 * _Complexity: O(1)_
 * @param array Pointer to the array to extend
 * @param p Pointer to the data to copy at the end of the array
 * @return Pointer to the newly created element (NULL in case of failure)
 */
void *array_list_push_back(array_list_t *array, void *value);

/**
 * @brief Remove the first element of the array
 *
 * If value is NULL the removed data will be lost, otherwise the data is copied
 * to the memory region pointed by value which should have been allocated with
 * array_list::size_bytes bytes.<br>
 *
 * _Complexity: O(n)_
 * @param[in] array pointer to the array
 * @param[out] value pointer to the remove data (if NULL the data will be lost)
 * @return FALSE iif the array was empty
 */
bool array_list_pop_front(array_list_t *array, void *value);

/**
 * @brief Remove the last element of the array
 *
 * If value is NULL the removed data will be lost, otherwise the data is copied
 * to the memory region pointed by value which should have been allocated with
 * array_list::size_bytes bytes.<br>
 *
 * _Complexity: O(1)_
 * @param[in] array pointer to the array
 * @param[out] value pointer to the remove data (if NULL the data will be freed)
 * @return FALSE iif the array was empty
 */
bool array_list_pop_back(array_list_t *array, void *value);

/**
 * @brief Swap two elements of the array
 *
 * _Complexity: O(1)_
 * @param[in] array pointer to the array
 * @param[in] a First index
 * @param[in] b Second index
 * @return -ERROR_NO_ERROR is the indices could be swapped
 */
int array_list_swap(array_list_t *array, unsigned a, unsigned b);

bool array_list_swap_and_pop_back(array_list_t *array, unsigned int i,
                                  void *value);

// IMPLEMENTATION

#ifdef CUTILS_ARRAY_LIST_IMPL

#include <stdlib.h>
#include <string.h>

#define array_list_init(type)                                                  \
  (array_list_t) {                                                             \
    .data = NULL, .capacity = 0, .size = 0, .size_bytes = sizeof(type)         \
  }

void array_list_deinit(array_list_t *array) {
  array->capacity = 0;
  array->size = 0;
  free(array->data);
  array->data = NULL;
}

bool array_list_empty(array_list_t *array) { return array->size == 0; }

static void ensure_sufficient_capacity(array_list_t *array) {
  if (array->size >= array->capacity) {
    if (array->capacity < ARRAY_LIST_MIN_CAPACITY)
      array->capacity = ARRAY_LIST_MIN_CAPACITY * array->size_bytes;
    else
      array->capacity *= 2;
    array->data = realloc(array->data, array->capacity * array->size_bytes);
  }
}

static void shift_right(array_list_t *array) {
  memmove(array->data + array->size_bytes, array->data,
          array->size * array->size_bytes);
}

void *array_list_push_front(array_list_t *array, void *value) {
  ensure_sufficient_capacity(array);
  array->size++;
  shift_right(array);
  void *ptr = array->data;
  memcpy(ptr, value, array->size_bytes);
  return ptr;
}

void *array_list_push_back(array_list_t *array, void *value) {
  ensure_sufficient_capacity(array);
  void *ptr = array->data + array->size_bytes * array->size;
  memcpy(ptr, value, array->size_bytes);
  array->size++;
  return ptr;
}

static void shift_left(array_list_t *array) {
  memmove(array->data, array->data + array->size_bytes,
          array->size * array->size_bytes);
}

bool array_list_pop_front(array_list_t *array, void *value) {
  if (array_list_empty(array))
    return false;

  if (value != NULL) {
    memcpy(value, array->data, array->size_bytes);
  }
  shift_left(array);
  array->size--;
  return true;
}

bool array_list_pop_back(array_list_t *array, void *value) {
  if (array_list_empty(array))
    return false;

  if (value != NULL) {
    char *ptr = array->data + array->size_bytes * (array->size - 1);
    memcpy(value, ptr, array->size_bytes);
  }
  array->size--;
  return true;
}

int array_list_swap(array_list_t *array, unsigned a, unsigned b) {
  when_null_ret(array, -ERROR_INVALID_PARAM1);
  when_false_ret(a < array->size, -ERROR_INVALID_PARAM2);
  when_false_ret(b < array->size, -ERROR_INVALID_PARAM3);
  if (a == b)
    return -ERROR_NO_ERROR;
  unsigned char *a_ref = get_array_list_ref(*array, a, unsigned char);
  unsigned char *b_ref = get_array_list_ref(*array, b, unsigned char);
  for (unsigned i = 0; i < array->size_bytes; i++) {
    unsigned char byte = a_ref[i];
    a_ref[i] = b_ref[i];
    b_ref[i] = byte;
  }
  return -ERROR_NO_ERROR;
}

bool array_list_swap_and_pop_back(array_list_t *array, unsigned i,
                                  void *value) {
  if (array_list_empty(array) == true)
    return false;
  int ret = array_list_swap(array, i, array->size - 1);
  when_false_ret(-ERROR_NO_ERROR == ret, FALSE);
  return array_list_pop_back(array, value);
}

#endif

/** @} */

#endif // !ARRAY_LIST_H
