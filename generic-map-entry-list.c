#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "generic-map-entry-list.h"
#include "buf-list.h"

generic_map_entry *generic_map_entry_list_payload(generic_map_entry_list *list)
{
    return buf_list_payload_typed((toy_buf_list *) list, generic_map_entry);
}

const generic_map_entry *generic_map_entry_list_payload_const(const generic_map_entry_list *list)
{
    return buf_list_payload_const_typed((toy_buf_list *) list, generic_map_entry);
}

void generic_map_entry_list_free(generic_map_entry_list *list)
{
    return buf_list_free((toy_buf_list *) list);
}
