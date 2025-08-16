#ifndef TOY_FUCTIONS_H
#define TOY_FUCTIONS_H 1

#include "str-types.h"
#include "val-types.h"
#include "function-types.h"
#include "symbol-table-types.h"

extern const toy_str_list INFINITE_PARAMS;

void func_dump(const toy_function *func);
#ifdef NDEBUG
#define func_assert_valid(func) do {} while (0)
#else /* ndef NDEBUG */
void func_assert_valid(const toy_function *func);
#endif /* ndef NDEBUG */
void func_free(toy_function *func);

#endif /* TOY_FUCTIONS_H */
