#ifndef EVAL_H_
#define EVAL_H_

#include "lexer.h"
#include "parse.h"
#include "exit.h"

Token eval_ast(ASTBinaryNode *);
int read_from_eval(char *, Token);

static inline token_kind set_kind(Token *t1, Token *t2) {
  return (t1->kind == NUMERIC_F || t2->kind == NUMERIC_F)
    ? NUMERIC_F
    : NUMERIC_I;
}

static inline double as_float(Token *tkn) {
  return tkn->kind == NUMERIC_F ? tkn->value.fval : (double) tkn->value.ival;
}

#ifdef EVAL_IMPL

// TODO: this pre-scans AST to determine if floating points
// will be necessary, this can be incorporated to ensure that
// no loss of floating point information is lost during the eval
// traversal.
/* static bool has_float(ASTBinaryNode *node) { */
/*   if (node == NULL) return false; */
/*   if (node->tkn.kind == NUMERIC_F) return true; */
/*   return has_float(node->lhs) || has_float(node->rhs); */
/* } */
// TODO: should be amended into a non-exposed inner function which
// accepts bool "has_float" as a flag, in has_float mode we want to
// coerce all longs into doubles.

Token eval_ast(ASTBinaryNode *node) {
  if (node->tkn.kind == NUMERIC_F || node->tkn.kind == NUMERIC_I) {
    return node->tkn;
  }
  Token a = eval_ast(node->lhs);
  Token b = eval_ast(node->rhs);

  Token result = (Token) { .kind = set_kind(&a, &b) };

  switch (node->tkn.kind) {
  case OP_ADD:
    if (result.kind == NUMERIC_I)
      result.value.ival = a.value.ival + b.value.ival;
    else result.value.fval = as_float(&a) + as_float(&b);
    break;
  case OP_SUB:
    if (result.kind == NUMERIC_I)
      result.value.ival = a.value.ival - b.value.ival;
    else result.value.fval = as_float(&a) - as_float(&b);
    break;
  case OP_MUL:
    if (result.kind == NUMERIC_I)
      result.value.ival = a.value.ival * b.value.ival;
    else result.value.fval = as_float(&a) * as_float(&b);
    break;
  case OP_DIV:
    if (result.kind == NUMERIC_I) {
      if (b.value.ival == 0) PANIC(EXIT_DIVIDE_BY_ZERO);
      result.value.ival = a.value.ival / b.value.ival;
    }
    else {
      if (b.value.fval == 0) PANIC(EXIT_DIVIDE_BY_ZERO);
      result.value.fval = as_float(&a) / as_float(&b);
    }
    break;
  default: break;
  }
  return result;
}

int read_from_eval(char *buffer, Token tkn) {
  switch (tkn.kind) {
  case NUMERIC_F: return sprintf(buffer, " = %g",  tkn.value.fval);
  case NUMERIC_I: return sprintf(buffer, " = %ld", tkn.value.ival);
  default: return -1;
  }
}
#endif // EVAL_IMPL
#endif // EVAL_H_
