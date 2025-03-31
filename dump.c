#include <stdio.h>

#include "dump.h"

void dump_list(FILE *f, toy_list *list)
{
    int printed_anything = 0;
    fputc('[', f);
    if (list) {
        for (toy_list *cur = list; cur; cur = cur->next) {
            if (printed_anything) {
                fputs(", ", f);
            } else {
                fputc(' ', f);
            }
            dump_expr(f, cur->expr);
            printed_anything = 1;
        }
        if (printed_anything) {
            fputc(' ', f);
        }
    }
    fputc(']', f);
}

void dump_str(FILE *f, const toy_str str)
{
    fputc('"', f);
    for (const char *p = str; *p; p++) {
        if (*p == '\'' || *p == '"' || *p == '\\') {
            fputc('\\', f);
        }
        fputc(*p, f);
    }
    fputc('"', f);
}

void dump_str_list(FILE *f, const toy_str_list *list)
{
    const toy_str_list *cur;
    if (list) {
        int output_something = 0;
        for (cur = list; cur; cur = cur->next) {
            if (output_something) {
                fputs(", ", f);
            }
            dump_str(f, cur->str);
            output_something = 1;
        }
    }
}

void dump_identifier(FILE *f, const toy_str str)
{
    fputs(str, f);
}

void dump_identifier_list(FILE *f, const toy_str_list *list)
{
    const toy_str_list *cur;
    if (list) {
        int output_something = 0;
        for (cur = list; cur; cur = cur->next) {
            if (output_something) {
                fputs(", ", f);
            }
            dump_identifier(f, cur->str);
            output_something = 1;
        }
    }
}

static void dump_binary_op(FILE *f, const toy_expr *arg1, const toy_expr *arg2, const char *op)
{
    fputc('(', f);
    dump_expr(f, arg1);
    fputs(op, f);
    dump_expr(f, arg2);
    fputc(')', f);
}

static void dump_assignment(FILE *f, const toy_str lhs, const toy_expr *rhs)
{
    dump_identifier(f, lhs);
    fputs(" = ", f);
    dump_expr(f, rhs);
}

void dump_expr(FILE *f, const toy_expr *expr) {
    if (expr) {
        switch (expr->type) {
        case EXPR_AND:
            dump_binary_op(f, expr->binary_op.arg1, expr->binary_op.arg2, " and ");
            break;
        case EXPR_ASSIGN:
            dump_assignment(f, expr->assignment.lhs, expr->assignment.rhs);
            break;
        case EXPR_BOOL:
            fputs(expr->bool ? "True" : "False", f);
            break;
        case EXPR_COLLECTION_LOOKUP:
            dump_identifier(f, expr->collection_lookup.lhs);
            fputc('[', f);
            dump_expr(f, expr->collection_lookup.rhs);
            fputc(']', f);
            break;
        case EXPR_COMMA:
            dump_binary_op(f, expr->binary_op.arg1, expr->binary_op.arg2, ", ");
            break;
        case EXPR_DIV:
            dump_binary_op(f, expr->binary_op.arg1, expr->binary_op.arg2, " / ");
            break;
        case EXPR_EQUAL:
            dump_binary_op(f, expr->binary_op.arg1, expr->binary_op.arg2, " == ");
            break;
        case EXPR_EXPONENT:
            dump_binary_op(f, expr->binary_op.arg1, expr->binary_op.arg2, " ** ");
            break;
        case EXPR_FUNC_CALL:
            fprintf(f, "%s(", expr->func_call.func_name);
            unsigned int output_something = 0;
            for (toy_list *arg = expr->func_call.args; arg; arg = arg->next) {
                if (output_something) {
                    fputs(", ", f);
                }
                dump_expr(f, arg->expr);
                output_something = 1;
            }
            fputs(")", f);
            break;
        case EXPR_FUNC_DECL:
            fprintf(f, "fun %s(", expr->func_decl.def.name);
            dump_identifier_list(f, expr->func_decl.def.param_names);
            fputs(") {\n", f);
            dump_stmts(f, expr->func_decl.def.code.stmts);
            fputs("}\n", f);
            break;
        case EXPR_GT:
            dump_binary_op(f, expr->binary_op.arg1, expr->binary_op.arg2, " > ");
            break;
        case EXPR_GTE:
            dump_binary_op(f, expr->binary_op.arg1, expr->binary_op.arg2, " >= ");
            break;
        case EXPR_IDENTIFIER:
            fprintf(f, "%s", expr->str);
            break;
        case EXPR_IN:
            dump_binary_op(f, expr->binary_op.arg1, expr->binary_op.arg2, " in ");
            break;
        case EXPR_LIST:
            dump_list(f, expr->list);
            break;
        case EXPR_LT:
            dump_binary_op(f, expr->binary_op.arg1, expr->binary_op.arg2, " < ");
            break;
        case EXPR_LTE:
            dump_binary_op(f, expr->binary_op.arg1, expr->binary_op.arg2, " <= ");
            break;
        case EXPR_MAP:
            dump_map(f, expr->map);
            break;
        case EXPR_MINUS:
            dump_binary_op(f, expr->binary_op.arg1, expr->binary_op.arg2, " - ");
            break;
        case EXPR_MODULUS:
            dump_binary_op(f, expr->binary_op.arg1, expr->binary_op.arg2, " % ");
            break;
        case EXPR_MUL:
            dump_binary_op(f, expr->binary_op.arg1, expr->binary_op.arg2, " * ");
            break;
        case EXPR_NEQUAL:
            dump_binary_op(f, expr->binary_op.arg1, expr->binary_op.arg2, " != ");
            break;
        case EXPR_NOT:
            fputs("not (", f);
            dump_expr(f, expr->unary_op.arg);
            fputs(")", f);
            break;
        case EXPR_NULL:
            fputs("null", f);
            break;
        case EXPR_NUM:
            fprintf(f, "%f", expr->num);
            break;
        case EXPR_OR:
            dump_binary_op(f, expr->binary_op.arg1, expr->binary_op.arg2, " or ");
            break;
        case EXPR_PLUS:
            dump_binary_op(f, expr->binary_op.arg1, expr->binary_op.arg2, " + ");
            break;
        case EXPR_POSTFIX_DECREMENT:
            dump_identifier(f, expr->postfix_decrement.id);
            fputs("--", f);
            break;
        case EXPR_POSTFIX_INCREMENT:
            dump_identifier(f, expr->postfix_increment.id);
            fputs("++", f);
            break;
        case EXPR_PREFIX_DECREMENT:
            fputs("--", f);
            dump_identifier(f, expr->prefix_decrement.id);
            break;
        case EXPR_PREFIX_INCREMENT:
            fputs("++", f);
            dump_identifier(f, expr->prefix_increment.id);
            break;
        case EXPR_STR:
            dump_str(f, expr->str);
            break;
        case EXPR_TERNARY:
            dump_expr(f, expr->ternary.condition);
            fputs(" ? ", f);
            dump_expr(f, expr->ternary.if_true);
            fputs(" : ", f);
            dump_expr(f, expr->ternary.if_false);
            break;
        case EXPR_UNEG:
            fputs("-", f);
            dump_expr(f, expr->unary_op.arg);
            break;
        default:
            invalid_expr_type(expr->type);
            break;
        }
    } else {
        fputs("null", f);
    }
}

void dump_stmt(FILE *f, const toy_stmt *stmt, int append_semicolon)
{
    switch (stmt->type) {
    case STMT_BLOCK:
        fputs("{\n", f);
        dump_stmts(f, stmt->block_stmt.block.stmts);
        fputs("}\n", f);
        break;
    case STMT_BREAK:
        fputs("break", f);
        if (append_semicolon) {
            fputc(';', f);
        }
        break;
    case STMT_CONTINUE:
        fputs("continue", f);
        if (append_semicolon) {
            fputc(';', f);
        }
        break;
    case STMT_EXPR:
        dump_expr(f, stmt->expr_stmt.expr);
        if (append_semicolon) {
            fputc(';', f);
        }
        break;
    case STMT_FOR:
        fputs("for (", f);
        if (stmt->for_stmt.at_start) {
            dump_stmt(f, stmt->for_stmt.at_start, 1);
        }
        fputc(' ', f);
        if (stmt->for_stmt.condition) {
            dump_expr(f, stmt->for_stmt.condition);
        } else {
            fputs("true", f);
        }
        fputs("; ", f);
        if (stmt->for_stmt.at_end) {
            dump_stmt(f, stmt->for_stmt.at_end, 0);
        }
        fputs(") {\n", f);
        if (stmt->for_stmt.body.stmts) {
            dump_stmts(f, stmt->for_stmt.body.stmts);
        }
        fputs("}", f);
        break;
    case STMT_FUNC_DECL:
        fprintf(f, "fun %s(", stmt->func_decl_stmt.def.name);
        dump_identifier_list(f, stmt->func_decl_stmt.def.param_names);
        fputs(") {\n", f);
        dump_stmts(f, stmt->func_decl_stmt.def.code.stmts);
        fputs("}", f);
        break;
    case STMT_IF:
        {
            toy_if_arm *arm = stmt->if_stmt.arms;
            fputs("if (", f);
            dump_expr(f, arm->condition);
            fputs(") {\n", f);
            dump_stmts(f, arm->code.stmts);
            fputs("}", f);
            for (arm = arm->next; arm; arm = arm->next) {
                fputs(" elseif (", f);
                dump_expr(f, arm->condition);
                fputs(") {\n", f);
                dump_stmts(f, arm->code.stmts);
                fputs("}", f);
            }
            if (stmt->if_stmt.elsepart.stmts) {
                fputs(" else {\n", f);
                dump_stmts(f, stmt->if_stmt.elsepart.stmts);
                fputs("}", f);
            }
        }
        break;
    case STMT_NULL:
        if (append_semicolon) {
            fputc(';', f);
        }
        break;
    case STMT_RETURN:
        fputs("return ", f);
        dump_expr(f, stmt->return_stmt.expr);
        if (append_semicolon) {
            fputc(';', f);
        }
        break;
    case STMT_VAR_DECL:
        fputs("var ", f);
        int output_something = 0;
        for (toy_var_decl *decl = stmt->var_decl_stmt; decl; decl = decl->next) {
            if (output_something) {
                fputs(", ", f);
            }
            fputs(decl->name, f);
            if (decl->value) {
                fputs(" = ", f);
                dump_expr(f, decl->value);
            }
            output_something = 1;
        }
        if (append_semicolon) {
            fputc(';', f);
        }
        break;
    case STMT_WHILE:
        fputs("while (\n", f);
        dump_expr(f, stmt->while_stmt.condition);
        fputs(") {\n", f);
        dump_stmts(f, stmt->while_stmt.body.stmts);
        fputs("}", f);
        break;
    default:
        invalid_stmt_type(stmt->type);
        break;
    }
}

void dump_stmts(FILE *f, const toy_stmt *stmts)
{
    for (const toy_stmt *s = stmts; s; s = s->next) {
        dump_stmt(f, s, 1);
        fputc('\n', f);
    }
}
