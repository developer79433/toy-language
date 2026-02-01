#ifndef TOY_MAP_PIPELINE_H
#define TOY_MAP_PIPELINE_H 1

#include "map-pipeline-types.h"

map_pipeline *map_pipeline_alloc(map_visitor *map_vis);
map_pipeline *map_pipeline_append(map_pipeline *pipeline, map_visitor *map_vis);
map_visitor *map_pipeline_payload(map_pipeline *pipeline);
enumeration_result map_pipeline_visit_map(map_pipeline *pipeline, generic_map *map);
void map_pipeline_free(map_pipeline *pipeline);
const_map_pipeline *const_map_pipeline_alloc(const_map_visitor *map_vis);
const_map_pipeline *const_map_pipeline_append(const_map_pipeline *pipeline, const_map_visitor *map_vis);
const_map_visitor *const_map_pipeline_payload(const_map_pipeline *pipeline);
enumeration_result const_map_pipeline_visit_map(const_map_pipeline *pipeline, const generic_map *map);
void const_map_pipeline_free(const_map_pipeline *pipeline);

#endif /* TOY_MAP_PIPELINE_H */
