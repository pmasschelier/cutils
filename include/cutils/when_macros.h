#ifndef TEST_MACROS_H
#define TEST_MACROS_H

#ifndef TEST_FAIL_FUNC

#ifdef TEST_FAIL_FUNC_NOCHECK
#define when_true_ret(...)
#define when_true_jmp(...)
#elif defined(TEST_FAIL_FUNC_RET)
#define when_true_ret(boolean, retval) \
	if (boolean)                       \
	return retval

#define when_true_jmp(boolean, retval, label) \
	do {                                      \
		if (boolean) {                        \
			ret = retval;                     \
			goto label;                       \
		}                                     \
	} while (0)
#else
#include <assert.h>
#define when_true_ret(boolean, ...) assert(!(boolean))
#define when_true_jmp(boolean, ...) assert(!(boolean))
#endif

#define when_false_ret(boolean, ...) when_true_ret(!(boolean), __VA_ARGS__)
#define when_null_ret(ptr, ...) when_true_ret(ptr == NULL, __VA_ARGS__)

#define when_false_jmp(boolean, ...) when_true_jmp(!(boolean), __VA_ARGS__)
#define when_null_jmp(ptr, ...) when_true_jmp(ptr == NULL, __VA_ARGS__)

#endif	// !TEST_FAIL_FUNC

#endif	// !TEST_MACROS_H
