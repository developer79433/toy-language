#ifndef TOY_LIST_PIPELINE_H
#define TOY_LIST_PIPELINE_H 1

#include "list-pipeline-types.h"

list_pipeline *list_pipeline_alloc(list_visitor *list_vis);
list_pipeline *list_pipeline_append(list_pipeline *pipeline, list_visitor *list_vis);
list_visitor *list_pipeline_payload(list_pipeline *pipeline);
enumeration_result list_pipeline_visit_item(list_pipeline *pipeline, generic_list *item, size_t item_index);
enumeration_result list_pipeline_visit_list(list_pipeline *pipeline, generic_list *list);
void list_pipeline_free(list_pipeline *pipeline);
const_list_pipeline *const_list_pipeline_alloc(const_list_visitor *list_vis);
const_list_pipeline *const_list_pipeline_append(const_list_pipeline *pipeline, const_list_visitor *list_vis);
const_list_visitor *const_list_pipeline_payload(const_list_pipeline *pipeline);
enumeration_result const_list_pipeline_visit_item(const_list_pipeline *pipeline, const generic_list *item, size_t item_index);
enumeration_result const_list_pipeline_visit_list(const_list_pipeline *pipeline, const generic_list *list);
void const_list_pipeline_free(const_list_pipeline *pipeline);

#endif /* TOY_LIST_PIPELINE_H */
