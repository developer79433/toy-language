#include <assert.h>

#include "list-pipeline.h"
#include "ptr-list.h"
#include "list-visitor.h"
#include "str-list.h"
#include "log.h"

list_visitor *list_pipeline_payload(list_pipeline *pipeline)
{
    return pipeline->visitor;
}

const_list_visitor *const_list_pipeline_payload(const_list_pipeline *pipeline)
{
    return pipeline->visitor;
}

list_pipeline *list_pipeline_alloc(list_visitor *list_vis)
{
    assert(sizeof(list_pipeline) == sizeof(toy_ptr_list));
    list_pipeline *pipeline = (list_pipeline *) ptr_list_alloc(list_vis);
    pipeline->visitor = list_vis;
    pipeline->next = NULL;
    return pipeline;
}

const_list_pipeline *const_list_pipeline_alloc(const_list_visitor *list_vis)
{
    assert(sizeof(const_list_pipeline) == sizeof(toy_ptr_list));
    const_list_pipeline *pipeline = (const_list_pipeline *) ptr_list_alloc(list_vis);
    pipeline->visitor = list_vis;
    pipeline->next = NULL;
    return pipeline;
}

list_pipeline *list_pipeline_append(list_pipeline *pipeline, list_visitor *list_vis)
{
    return (list_pipeline *) ptr_list_append((toy_ptr_list *) pipeline, list_vis);
}

const_list_pipeline *const_list_pipeline_append(const_list_pipeline *pipeline, const_list_visitor *list_vis)
{
    return (const_list_pipeline *) ptr_list_append((toy_ptr_list *) pipeline, list_vis);
}

void list_pipeline_free(list_pipeline *pipeline)
{
    ptr_list_free((toy_ptr_list *) pipeline);
}

void const_list_pipeline_free(const_list_pipeline *pipeline)
{
    ptr_list_free((toy_ptr_list *) pipeline);
}

typedef struct list_pipeline_item_visitor_struct {
    list_visitor list_vis;
    generic_list *item;
    size_t item_index;
} list_pipeline_item_visitor;

static item_callback_result visit_pipeline_entry(list_pipeline_item_visitor *pipeline_item_vis, size_t pipeline_index, list_pipeline *pipeline_item)
{
    list_visitor *visitor_from_pipeline = list_pipeline_payload(pipeline_item);
    return visitor_from_pipeline->visit_entry(visitor_from_pipeline, pipeline_item_vis->item_index, pipeline_item_vis->item);
}

enumeration_result list_pipeline_visit_item(list_pipeline *pipeline, generic_list *item, size_t item_index)
{
    list_pipeline_item_visitor pipeline_item_vis = {
        .list_vis.visit_entry = (list_entry_visit_func) visit_pipeline_entry,
        .list_vis.previous_item = NULL,
        .item = item,
        .item_index = item_index
    };
    return list_visitor_visit_list((list_visitor *) &pipeline_item_vis, (generic_list *) pipeline);
}

typedef struct const_list_pipeline_item_visitor_struct {
    const_list_visitor list_vis;
    const generic_list *item;
    size_t item_index;
} const_list_pipeline_item_visitor;

static item_callback_result const_visit_pipeline_entry(const_list_pipeline_item_visitor *pipeline_item_vis, size_t pipeline_index, const_list_pipeline *pipeline_item)
{
    const_list_visitor *visitor_from_pipeline = const_list_pipeline_payload(pipeline_item);
    return visitor_from_pipeline->visit_entry(visitor_from_pipeline, pipeline_item_vis->item_index, pipeline_item_vis->item);
}

enumeration_result const_list_pipeline_visit_item(const_list_pipeline *pipeline, const generic_list *item, size_t item_index)
{
    const_list_pipeline_item_visitor pipeline_item_vis = {
        .list_vis.visit_entry = (const_list_entry_visit_func) const_visit_pipeline_entry,
        .list_vis.previous_item = NULL,
        .item = item,
        .item_index = item_index
    };
    return const_list_visitor_visit_list((const_list_visitor *) &pipeline_item_vis, (const generic_list *) pipeline);
}

typedef struct list_pipeline_visitor_struct {
    list_visitor list_vis;
    list_pipeline *pipeline;
} list_pipeline_visitor;

static item_callback_result list_entry_visit_pipeline(list_pipeline_visitor *list_pipeline_vis, size_t item_index, generic_list *item)
{
    enumeration_result enum_res = list_pipeline_visit_item(list_pipeline_vis->pipeline, item, item_index);
    if (ENUMERATION_INTERRUPTED == enum_res) {
        return STOP_ENUMERATION;
    }
    assert(ENUMERATION_COMPLETE == enum_res);
    return CONTINUE_ENUMERATION;
}

enumeration_result list_pipeline_visit_list(list_pipeline *pipeline, generic_list *list)
{
    list_pipeline_visitor list_pipeline_vis = {
        .list_vis.visit_entry = (list_entry_visit_func) list_entry_visit_pipeline,
        .list_vis.previous_item = NULL,
        .pipeline = pipeline
    };
    return list_visitor_visit_list((list_visitor *) &list_pipeline_vis, list);
}

typedef struct const_list_pipeline_visitor_struct {
    const_list_visitor list_vis;
    const_list_pipeline *pipeline;
} const_list_pipeline_visitor;

static item_callback_result const_list_entry_visit_pipeline(const_list_pipeline_visitor *list_pipeline_vis, size_t item_index, const generic_list *item)
{
    enumeration_result enum_res = const_list_pipeline_visit_item(list_pipeline_vis->pipeline, item, item_index);
    if (ENUMERATION_INTERRUPTED == enum_res) {
        return STOP_ENUMERATION;
    }
    assert(ENUMERATION_COMPLETE == enum_res);
    return CONTINUE_ENUMERATION;
}

enumeration_result const_list_pipeline_visit_list(const_list_pipeline *pipeline, const generic_list *list)
{
    const_list_pipeline_visitor list_pipeline_vis = {
        .list_vis.visit_entry = (const_list_entry_visit_func) const_list_entry_visit_pipeline,
        .list_vis.previous_item = NULL,
        .pipeline = pipeline
    };
    return const_list_visitor_visit_list((const_list_visitor *) &list_pipeline_vis, list);
}

static item_callback_result print_entry_1(list_visitor *list_vis, size_t index, const toy_str_list *item)
{
    toy_str str = str_list_payload_const(item);
    log_printf("list-pipeline: item 1: %s\n", str);
    return CONTINUE_ENUMERATION;
}

static item_callback_result print_entry_2(list_visitor *list_vis, size_t index, const toy_str_list *item)
{
    toy_str str = str_list_payload_const(item);
    log_printf("list-pipeline: item 2: %s\n", str);
    return CONTINUE_ENUMERATION;
}

void test_pipelines()
{
    list_visitor print_vis_1 = { .visit_entry = (list_entry_visit_func) print_entry_1, .previous_item = NULL };
    list_visitor print_vis_2 = { .visit_entry = (list_entry_visit_func) print_entry_2, .previous_item = NULL };
    list_pipeline *pipeline = list_pipeline_alloc(&print_vis_1);
    pipeline = list_pipeline_append(pipeline, &print_vis_2);
    toy_str_list *str_list = str_list_alloc("First item");
    enumeration_result res = list_pipeline_visit_list(pipeline, (generic_list *) str_list);
    assert(ENUMERATION_COMPLETE == res);
    list_pipeline_free(pipeline);
    str_list_free(str_list);
}
