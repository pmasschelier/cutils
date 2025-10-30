#ifndef CUTILS_ALLOC_H
#define CUTILS_ALLOC_H

#ifndef CUTILS_NO_STD

#if !defined(CUTILS_alloc) || !defined(CUTILS_realloc) || !defined(CUTILS_dealloc)
#ifdef CUTILS_NO_STD
#error "When the CUTILS_NO_STD is defined, CUTILS_malloc, CUTILS_realloc and CUTILS_dealloc should be provided"
#endif
#include <stdlib.h>
#endif
#ifndef CUTILS_alloc
#define CUTILS_alloc malloc
#endif
#ifndef CUTILS_realloc
#define CUTILS_realloc realloc
#endif
#ifndef CUTILS_dealloc
#define CUTILS_dealloc free
#endif

#endif

#endif //CUTILS_ALLOC_H
