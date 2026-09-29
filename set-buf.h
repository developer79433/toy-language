#ifndef TOY_SET_BUF_H
#define TOY_SET_BUF_H 1

#include "str.h"
#include "set-buf-types.h"

set_buf *set_buf_alloc(void);
void set_buf_init(set_buf *set);
void set_buf_dump(set_buf *set);
toy_bool set_buf_contains(const set_buf *set, const void *buf, size_t size);
void set_buf_add(set_buf *set, const void *buf, size_t size);
void set_buf_remove(set_buf *set, const void *buf, size_t size);
void set_buf_free(set_buf *set);

#endif /* TOY_SET_BUF_H */
