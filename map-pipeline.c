#include <assert.h>

#include "map-pipeline.h"
#include "ptr-list.h"
#include "map-visitor.h"
#include "list-visitor.h"

map_visitor *map_pipeline_payload(map_pipeline *pipeline)
{
    return pipeline->visitor;
}

const_map_visitor *const_map_pipeline_payload(const_map_pipeline *pipeline)
{
    return pipeline->visitor;
}

map_pipeline *map_pipeline_alloc(map_visitor *map_vis)
{
    assert(sizeof(map_pipeline) == sizeof(toy_ptr_list));
    map_pipeline *pipeline = (map_pipeline *) ptr_list_alloc(map_vis);
    pipeline->visitor = map_vis;
    pipeline->next = NULL;
    return pipeline;
}

const_map_pipeline *const_map_pipeline_alloc(const_map_visitor *map_vis)
{
    assert(sizeof(const_map_pipeline) == sizeof(toy_ptr_list));
    const_map_pipeline *pipeline = (const_map_pipeline *) ptr_list_alloc(map_vis);
    pipeline->visitor = map_vis;
    pipeline->next = NULL;
    return pipeline;
}

map_pipeline *map_pipeline_append(map_pipeline *pipeline, map_visitor *map_vis)
{
    return (map_pipeline *) ptr_list_append((toy_ptr_list *) pipeline, map_vis);
}

const_map_pipeline *const_map_pipeline_append(const_map_pipeline *pipeline, const_map_visitor *map_vis)
{
    return (const_map_pipeline *) ptr_list_append((toy_ptr_list *) pipeline, map_vis);
}

void map_pipeline_free(map_pipeline *pipeline)
{
    ptr_list_free((toy_ptr_list *) pipeline);
}

void const_map_pipeline_free(const_map_pipeline *pipeline)
{
    ptr_list_free((toy_ptr_list *) pipeline);
}

typedef struct map_pipeline_item_visitor_struct {
    list_visitor list_vis;
    generic_map_entry *entry;
} map_pipeline_item_visitor;

typedef struct map_pipeline_visitor_struct {
    map_visitor map_vis;
    map_pipeline *pipeline;
} map_pipeline_visitor;

static item_callback_result visit_pipeline_entry(map_pipeline_item_visitor *pipeline_item_vis, size_t pipeline_index, map_pipeline *pipeline_item)
{
    map_visitor *visitor_from_pipeline = map_pipeline_payload(pipeline_item);
    return visitor_from_pipeline->visit_entry(visitor_from_pipeline, pipeline_item_vis->entry);
}

static item_callback_result map_entry_visit_pipeline(map_pipeline_visitor *map_pipeline_vis, generic_map_entry *entry)
{
    map_pipeline_item_visitor pipeline_item_vis = {
        .list_vis.visit_entry = (list_entry_visit_func) visit_pipeline_entry,
        .list_vis.prev_item = NULL,
        .entry = entry
    };
    enumeration_result res = list_visitor_visit_list((list_visitor *) &pipeline_item_vis, (generic_list *) map_pipeline_vis->pipeline);
    if (res == ENUMERATION_INTERRUPTED) {
        return STOP_ENUMERATION;
    }
    assert(ENUMERATION_COMPLETE == res);
    return CONTINUE_ENUMERATION;
}

enumeration_result map_pipeline_visit_map(map_pipeline *pipeline, generic_map *map)
{
    map_pipeline_visitor map_pipeline_vis = {
        .map_vis.visit_entry = (map_entry_visit_func) map_entry_visit_pipeline,
        .pipeline = pipeline
    };
    return map_visitor_visit_map((map_visitor *) &map_pipeline_vis, map);
}

typedef struct const_map_pipeline_item_visitor_struct {
    const_list_visitor list_vis;
    const generic_map_entry *entry;
} const_map_pipeline_item_visitor;

typedef struct const_map_pipeline_visitor_struct {
    const_map_visitor map_vis;
    const_map_pipeline *pipeline;
} const_map_pipeline_visitor;

static item_callback_result const_visit_pipeline_entry(const_map_pipeline_item_visitor *pipeline_item_vis, size_t pipeline_index, const_map_pipeline *pipeline_item)
{
    const_map_visitor *visitor_from_pipeline = const_map_pipeline_payload(pipeline_item);
    return visitor_from_pipeline->visit_entry(visitor_from_pipeline, pipeline_item_vis->entry);
}

static item_callback_result const_map_entry_visit_pipeline(const_map_pipeline_visitor *map_pipeline_vis, const generic_map_entry *entry)
{
    const_map_pipeline_item_visitor pipeline_item_vis = {
        .list_vis.visit_entry = (const_list_entry_visit_func) const_visit_pipeline_entry,
        .list_vis.prev_item = NULL,
        .entry = entry
    };
    enumeration_result res = const_list_visitor_visit_list((const_list_visitor *) &pipeline_item_vis, (const generic_list *) map_pipeline_vis->pipeline);
    if (res == ENUMERATION_INTERRUPTED) {
        return STOP_ENUMERATION;
    }
    assert(ENUMERATION_COMPLETE == res);
    return CONTINUE_ENUMERATION;
}

enumeration_result const_map_pipeline_visit_map(const_map_pipeline *pipeline, const generic_map *map)
{
    const_map_pipeline_visitor map_pipeline_vis = {
        .map_vis.visit_entry = (const_map_entry_visit_func) const_map_entry_visit_pipeline,
        .pipeline = pipeline
    };
    return const_map_visitor_visit_map((const_map_visitor *) &map_pipeline_vis, map);
}
