#ifndef TOY_SET_STR_H
#define TOY_SET_STR_H 1

#include "str.h"
#include "set-str-types.h"

set_str *set_str_alloc(void);
void set_str_init(set_str *set);
void set_str_dump(set_str *set);
toy_bool set_str_contains(const set_str *set, const toy_str str);
void set_str_add(set_str *set, const toy_str str);
void set_str_remove(set_str *set, const toy_str str);
void set_str_free(set_str *set);

#endif /* TOY_SET_STR_H */
