#ifndef TOY_INTERP_H
#define TOY_INTERP_H 1

#include "str-types.h"
#include "val-types.h"
#include "val-list-types.h"
#include "expr-list-types.h"
#include "function-types.h"
#include "interp-types.h"

struct toy_stmt_struct;
typedef struct toy_stmt_struct toy_stmt;
struct toy_block_struct;
typedef struct toy_block_struct toy_block;

toy_interp *alloc_interp(const toy_stmt *program);
void free_interp(toy_interp *interp);
void eval_expr(toy_interp *interp, toy_val *result, const toy_expr *expr);
enum run_stmt_result run_stmt(toy_interp *interp, const toy_stmt *stmt);
enum run_stmt_result run_current_block(toy_interp *interp);
toy_bool convert_to_bool(const toy_val *val);
int lookup_identifier(toy_interp *interp, toy_val *result, const toy_str name);
void call_func(toy_interp *interp, toy_val *result, toy_str func_name, toy_expr_list *args);
void run_toy_function_expr_list(toy_interp *interp, toy_val *result, toy_func_def *def, toy_expr_list *args);
void run_toy_function_val_list(toy_interp *interp, toy_val *result, toy_func_def *def, const toy_val_list *args);
const char *frame_type_name(frame_type type);

#endif /* TOY_INTERP_H */
