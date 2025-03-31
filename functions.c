#include <string.h>

#include "util.h"
#include "functions.h"
#include "list.h"
#include "dump.h"
#include "constants.h"

typedef struct predefined_function_struct {
    toy_str name;
    int num_params;
    predefined_func_addr func;
    const toy_char **param_names;
} predefined_function;

#define INFINITE_PARAMS -1

static void predefined_list_len(toy_interp *interp, toy_expr *result, toy_list *args)
{
    if (args->next) {
        too_many_arguments(1, args);
    }
    toy_expr *arg1 = args->expr;
    if (arg1->type != EXPR_LIST) {
        invalid_operand(EXPR_LIST, arg1);
    }
    result->type = EXPR_NUM;
    result->num = list_len(arg1->list);
}

static const char *predefined_list_len_arg_names[] = {
    "list"
};

static void predefined_print(toy_interp *interp, toy_expr *result, toy_list *args)
{
    for (toy_list *arg = args; arg; arg = arg->next) {
        toy_expr arg_result;
        eval_expr(interp, &arg_result, arg->expr);
        if (arg_result.type == EXPR_STR) {
            print_str(stderr, arg_result.str);
        } else {
            dump_expr(stderr, &arg_result);
        }
        fputc('\n', stderr);
    }
    *result = null_expr;
}

static predefined_function predefined_functions[] = {
    { .name = "len",   .num_params = 1,               .func = predefined_list_len, .param_names = predefined_list_len_arg_names },
    { .name = "print", .num_params = INFINITE_PARAMS, .func = predefined_print,    .param_names = NULL }
};

predefined_func_addr lookup_predefined_function(const toy_str name)
{
    for (
        const predefined_function *function = &predefined_functions[0];
        function < &predefined_functions[ELEMENTSOF(predefined_functions)];
        function++)
    {
        if (0 == strcasecmp(function->name, name)) {
            return function->func;
        }
    }
    return 0;
}
