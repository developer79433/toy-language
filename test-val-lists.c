#include <assert.h>

#include "test-val-lists.h"
#include "val.h"
#include "val-list.h"

void test_val_lists(void)
{
    toy_val first_elem = { .type = VAL_BOOL, .boolean = TOY_TRUE };
    toy_val_list *val_list = val_list_alloc(&first_elem);
    assert(val_list);
    assert(val_list->val.type == VAL_BOOL);
    assert(val_list->val.boolean == TOY_TRUE);
    assert(val_list->next == NULL);
    assert(1 == val_list_len(val_list));
    toy_val new_elem = { .type = VAL_BOOL, .boolean = TOY_FALSE };
    toy_val_list *retval = val_list_append(val_list, &new_elem);
    assert(retval == val_list);
    assert(retval->next);
    assert(!retval->next->next);
    assert(retval->next->val.type == VAL_BOOL);
    assert(retval->next->val.boolean == TOY_FALSE);
    assert(2 == val_list_len(retval));
    /* TODO: Test enumeration */
    val_list_free(retval);
}
