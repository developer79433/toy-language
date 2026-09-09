#include <assert.h>

#include "decl-ref-list.h"
#include "list-visitor.h"
#include "decl-ref.h"
#include "buf-list.h"
#include "log.h"

decl_ref *decl_ref_list_payload(decl_ref_list *item)
{
    return &item->ref;
}

const decl_ref *decl_ref_list_payload_const(const decl_ref_list *item)
{
    return &item->ref;
}

static item_callback_result decl_ref_list_entry_dump(const_list_visitor *dump_vis, size_t index, const decl_ref_list *item)
{
    const decl_ref *ref = decl_ref_list_payload_const(item);
    decl_ref_dump(ref);
    return CONTINUE_ENUMERATION;
}

static item_callback_result decl_ref_list_entry_assert_valid(const_list_visitor *list_vis, size_t index, const decl_ref_list *item)
{
    const decl_ref *ref = decl_ref_list_payload_const(item);
    decl_ref_assert_valid(ref);
    return CONTINUE_ENUMERATION;
}

void decl_ref_list_assert_valid(const decl_ref_list *list)
{
    const_list_visitor vis = { .visit_entry = (const_list_entry_visit_func) decl_ref_list_entry_assert_valid, .prev_item = NULL };
    enumeration_result res = const_list_visitor_visit_list(&vis, (generic_list *) list);
    assert(ENUMERATION_COMPLETE == res);
}

void decl_ref_list_dump(const decl_ref_list *list)
{
    decl_ref_list_assert_valid(list);
    log_debug("decl_ref_list {\n");
    const_list_visitor dump_vis = { .visit_entry = (const_list_entry_visit_func) decl_ref_list_entry_dump, .prev_item = NULL };
    enumeration_result res = const_list_visitor_visit_list(&dump_vis, (const generic_list *) list);
    assert(ENUMERATION_COMPLETE == res);
    log_debug("}\n");
}

decl_ref_list *decl_ref_list_alloc(decl_ref *ref)
{
    decl_ref_assert_valid(ref);
    decl_ref_list *list = (decl_ref_list *) buf_list_alloc(ref, sizeof(*ref));
    decl_ref_list_assert_valid(list);
    return list;
}

decl_ref_list *decl_ref_list_prepend(decl_ref_list *list, decl_ref *ref)
{
    decl_ref_assert_valid(ref);
    decl_ref_list_assert_valid(list);
    return (decl_ref_list *) buf_list_prepend((toy_buf_list *) list, ref, sizeof(*ref));
}

typedef struct decl_ref_visitor_struct {
    list_visitor list_vis;
    toy_str desired_name;
    decl_ref *match;
} decl_ref_visitor;

static enumeration_result decl_ref_visitor_visit_decl(decl_ref_visitor *decl_ref_vis, size_t index, decl_ref_list *item)
{
    decl_ref *ref = decl_ref_list_payload(item);
    if (decl_ref_name_matches(ref, decl_ref_vis->desired_name)) {
        decl_ref_vis->match = ref;
        return STOP_ENUMERATION;
    }
    return CONTINUE_ENUMERATION;
}

decl_ref *decl_ref_list_find_name(decl_ref_list *list, toy_str desired_name)
{
    decl_ref_visitor decl_ref_vis = {
        .list_vis.visit_entry = (list_entry_visit_func) decl_ref_visitor_visit_decl,
        .list_vis.prev_item = NULL,
        .desired_name = desired_name,
        .match = NULL
    };
    enumeration_result res = list_visitor_visit_list(&decl_ref_vis.list_vis, (generic_list *) list);
    decl_ref *ref = decl_ref_vis.match;
    assert(
        (ENUMERATION_COMPLETE == res && !ref)
        ||
        (ENUMERATION_INTERRUPTED == res && ref)
    );
    if (ref) {
        decl_ref_assert_valid(ref);
    }
    return ref;
}
