#ifndef CUTILS_STRING_H
#define CUTILS_STRING_H

#include <cutils/array.h>

DEFINE_ARRAY_TYPE(char)
typedef char_array_t string_t;

#define string_view(c_str) (string_view_t) { .str = str, .len = strlen(str) }
#define string_free array_free_char

typedef struct {
    char* str;
    size_t len;
} string_view_t;

UNUSED
static string_t string_from(const string_view_t copy) {
    string_t str = EMPTY_ARRAY;
    // Allocate copy.len char + NULL terminating byte
    array_append_char(&str, copy.len + 1);
    strncpy(str.data, copy.str, copy.len);
    str.data[copy.len] = '\0';
    return str;
}

#endif //CUTILS_STRING_H
