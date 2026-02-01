#ifndef TOY_LIST_PIPELINE_TYPES_H
#define TOY_LIST_PIPELINE_TYPES_H 1

#include "list-visitor-types.h"

struct list_pipeline_struct;
typedef struct list_pipeline_struct list_pipeline;
struct list_pipeline_struct {
    list_pipeline *next;
    list_visitor *visitor;
};

struct const_list_pipeline_struct;
typedef struct const_list_pipeline_struct const_list_pipeline;
struct const_list_pipeline_struct {
    const_list_pipeline *next;
    const_list_visitor *visitor;
};

#endif /* TOY_LIST_PIPELINE_TYPES_H */
