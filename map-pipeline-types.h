#ifndef TOY_MAP_PIPELINE_TYPES_H
#define TOY_MAP_PIPELINE_TYPES_H 1

#include "map-visitor-types.h"

struct map_pipeline_struct;
typedef struct map_pipeline_struct map_pipeline;
struct map_pipeline_struct {
    map_pipeline *next;
    map_visitor *visitor;
};

struct const_map_pipeline_struct;
typedef struct const_map_pipeline_struct const_map_pipeline;
struct const_map_pipeline_struct {
    const_map_pipeline *next;
    const_map_visitor *visitor;
};

#endif /* TOY_MAP_PIPELINE_TYPES_H */
