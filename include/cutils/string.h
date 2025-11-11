#ifndef CUTILS_STRING_H
#define CUTILS_STRING_H

#include <cutils/array.h>

DEFINE_ARRAY_TYPE(char)
typedef char_array_t string_t;

#define string_view(c_str) (string_view_t) { .str = c_str, .len = strlen(c_str) }
#define string_free array_free_char
#define EMPTY_STRING EMPTY_ARRAY(char)
#define EMPTY_STRING_VIEW (string_view_t) { .str = "", .len = 0 }

typedef struct {
    char* str;
    size_t len;
} string_view_t;

UNUSED NODISCARD
static string_t string_from(const string_view_t copy) {
    string_t str = EMPTY_STRING;
    // Allocate copy.len char + NULL terminating byte
    array_append_char(&str, copy.len + 1);
    strncpy(str.data, copy.str, copy.len);
    str.data[copy.len] = '\0';
    return str;
}

UNUSED NODISCARD
static string_view_t string_slice(const string_t str, int start, int end) {
    // if start < 0 start from the end of the string (-1 starts at the last character)
    start %= str.length;
    if (start < 0) start += str.length;
    // if end < 0 ends from the end of the string (-1 stop at the end of the string)
    end %= str.length;
    if (end < 0) start += str.length + 1;
    if (end <= start) return EMPTY_STRING_VIEW;
    return (string_view_t) {
        .str = str.data + start,
        .len = end - start
    };
}

#endif //CUTILS_STRING_H
