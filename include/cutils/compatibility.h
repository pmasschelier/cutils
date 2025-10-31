#ifndef CUTILS_COMPATIBILITY_H
#define CUTILS_COMPATIBILITY_H

#if __STDC_VERSION__ < 202311L
#include <stdbool.h>
#include <stddef.h>
#define CONSTEXPR const
#ifdef __GNUC__
#define UNUSED __attribute__((unused))
#else
#define UNUSED
#endif
#define NODISCARD

#else
#ifndef NULL
#define NULL nullptr
#endif
#define CONSTEXPR constexpr
#define UNUSED [[maybe_unused]]
#define NODISCARD [[nodiscard]]
#endif

#endif //CUTILS_COMPATIBILITY_H
