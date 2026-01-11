#include <assert.h>

#include "str.h"
#include "expr.h"
#include "buf-list.h"
#include "var-decl.h"
#include "var-decl-list.h"
#include "log.h"

size_t var_decl_list_len(const toy_var_decl_list *list)
{
    return buf_list_len((const toy_buf_list *) list);
}

toy_var_decl *var_decl_list_payload(toy_var_decl_list *list)
{
    assert(&list->decl == buf_list_payload_typed((toy_buf_list *) list, toy_var_decl));
    return buf_list_payload_typed((toy_buf_list *) list, toy_var_decl);
}

const toy_var_decl *var_decl_list_payload_const(const toy_var_decl_list *list)
{
    assert(&list->decl == buf_list_payload_const_typed((const toy_buf_list *) list, toy_var_decl));
    return buf_list_payload_const_typed((const toy_buf_list *) list, toy_var_decl);
}

toy_var_decl_list *var_decl_list_alloc(toy_var_decl *decl)
{
    assert(offsetof(toy_var_decl_list, decl) == offsetof(toy_buf_list, c));
    assert(offsetof(toy_var_decl_list, next) == offsetof(generic_list, next));
    assert(offsetof(toy_var_decl_list, next) == offsetof(toy_buf_list, next));
    toy_buf_list *buf_list = buf_list_alloc(decl, sizeof(*decl));
    toy_var_decl_list *decl_list = (toy_var_decl_list *) buf_list;
    assert(decl_list->decl.name == decl->name);
    assert(decl_list->decl.value == decl->value);
    return decl_list;
}

enumeration_result var_decl_list_foreach(toy_var_decl_list *list, toy_var_decl_list_item_callback callback, void *cookie)
{
    return buf_list_foreach((toy_buf_list *) list, (buf_list_item_callback) callback, cookie);
}

enumeration_result var_decl_list_foreach_const(const toy_var_decl_list *list, const_toy_var_decl_list_item_callback callback, void *cookie)
{
    return buf_list_foreach_const((const toy_buf_list *) list, (const_buf_list_item_callback) callback, cookie);
}

typedef struct var_decl_dump_cb_args_struct {
    toy_bool output_something;
} var_decl_dump_cb_args;

static item_callback_result var_decl_dump_callback(void *cookie, size_t index, const toy_var_decl_list *item)
{
    var_decl_dump_cb_args *args = (var_decl_dump_cb_args *) cookie;
    const toy_var_decl *var_decl = var_decl_list_payload_const(item);
    if (args->output_something) {
        log_puts(", ");
    }
    var_decl_dump(var_decl);
    args->output_something = TOY_TRUE;
    return CONTINUE_ENUMERATION;
}

void var_decl_list_dump(const toy_var_decl_list *list)
{
    var_decl_dump_cb_args args = { .output_something = TOY_FALSE };
    enumeration_result res = var_decl_list_foreach_const(list, var_decl_dump_callback, &args);
    assert(ENUMERATION_COMPLETE == res);
}

void var_decl_list_free(toy_var_decl_list *list)
{
    buf_list_free((toy_buf_list *) list);
}

toy_var_decl_list *var_decl_list_concat(toy_var_decl_list *list, toy_var_decl_list *new_list)
{
    assert(offsetof(toy_var_decl_list, next) == offsetof(toy_buf_list, next));
    return (toy_var_decl_list *) buf_list_concat((toy_buf_list *) list, (toy_buf_list *) new_list);
}

toy_var_decl_list *var_decl_list_append(toy_var_decl_list *list, toy_var_decl *new_var_decl)
{
    toy_var_decl_list *new_list = var_decl_list_alloc(new_var_decl);
    assert(new_list->decl.name == new_var_decl->name);
    assert(new_list->decl.value == new_var_decl->value);
    assert(NULL == new_list->next);
    return var_decl_list_concat(list, new_list);
}

static item_callback_result decl_assert_valid_cb(void *cookie, size_t index, const toy_var_decl_list *item)
{
    const toy_var_decl *var_decl = var_decl_list_payload_const(item);
    var_decl_assert_valid(var_decl);
    return CONTINUE_ENUMERATION;
}

void var_decl_list_assert_valid(const toy_var_decl_list *list)
{
    enumeration_result res = var_decl_list_foreach_const(list, decl_assert_valid_cb, NULL);
    assert(ENUMERATION_COMPLETE == res);
}
