#include "mpc.h"
#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
#include <string.h>

static char input[2048];
char *readline(const char *prompt) {
  fputs(prompt, stdout);
  fgets(input, 2048, stdin);

  printf("test");
  char *copy = malloc(strlen(input));
  strcpy(copy, input);
  copy[strlen(copy) - 1] = '\0';
  /* USE strcpy instead!!
     for(size_t i = 0; i >= 2048; ++i){
     copy[i] = input[i];
     if(input[i] == '\0') break;
     }
  */

  assert(copy[strlen(copy) - 1] == '\0');
  return copy;
}

void add_history(char *in) {}

#else
#include <readline/history.h>
#include <readline/readline.h>

#endif /*_WIN32 */
/* Create Enumeration of Possible Error Types */
enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM };

/* Create Enumeration of Possible lval Types */
enum { LVAL_NUM, LVAL_ERR, LVAL_SYM, LVAL_SEXPR };

/* Declare New lval Struct */
typedef struct lval {
  int type;
  long num;
  char *err;
  char *sym;
  int count;
  struct lval **cell;
} lval;

/* Create a new number type lval */
lval *lval_num(long x) {
  lval *v = malloc(sizeof(lval));
  (*v).type = LVAL_NUM;
  (*v).num = x;
  return v;
}

/* Create a new error type lval */
lval *lval_err(char *x) {
  lval *v = malloc(sizeof(lval));
  (*v).type = LVAL_ERR;
  (*v).err = malloc(strlen(x) + 1);
  strcpy(v->err, x);
  return v;
}
lval *lval_sym(char *sym) {
  lval *v = malloc(sizeof(lval));
  (*v).type = LVAL_SYM;
  (*v).sym = malloc(strlen(sym) + 1);
  strcpy((*v).sym, sym);
  return v;
}
lval *lval_sexpr(void) {
  lval *v = malloc(sizeof(lval));
  (*v).type = LVAL_SEXPR;
  (*v).count = 0;
  (*v).cell = NULL;
  return v;
}

void lval_del(lval *v) {
  switch (v->type) {
  case LVAL_NUM:
    break;
  case LVAL_SYM:
    free(v->sym);
    break;
  case LVAL_ERR:
    free(v->err);
    break;
  case LVAL_SEXPR:
    for (size_t i = 0; i < v->count; ++i) {
      free(v->cell[i]);
    }
    free(v->cell);
    break;
  }
  free(v);
}

lval *lval_read_num(mpc_ast_t *t) {
  errno = 0;
  long x = strtol(t->contents, NULL, 10);
  return errno != ERANGE ? lval_num(x) : lval_err("Invalid number");
}
lval *lval_add(lval *v, lval *x) {
  v->count++;
  v->cell = realloc(v->cell, sizeof(lval *) * v->count);
  v->cell[v->count - 1] = x;
  return v;
}

lval *lval_read(mpc_ast_t *t) {
  lval *x = NULL;
  if (strstr(t->tag, "number")) { return lval_read_num(t); }
  if (strstr(t->tag, "symbol")) { return lval_sym(t->contents); }
  if (strcmp(t->tag, ">") == 0) { x = lval_sexpr(); }
  if (strcmp(t->tag, "sexpr")) { x = lval_sexpr(); }

  for (size_t i = 0; i < t->children_num; ++i) {
    if (strcmp(t->children[i]->contents, "(") == 0) { continue; }
    if (strcmp(t->children[i]->contents, ")") == 0) { continue; }
    if (strcmp(t->children[i]->tag, "regex") == 0) { continue; }
    x = lval_add(x, lval_read(t->children[i]));
  }

  return x;
}

void lval_print(lval *v);
void lval_expr_print(lval *v, char open, char close);

void lval_expr_print(lval *v, char open, char close){
    putchar(open);
    for(size_t i = 0; i < v->count; ++i){
        lval_print(v->cell[i]);
        if ( i != (v-> count-1))
            putchar(' ');
        {
    }
    putchar(close);
}
}
/* Print an "lval" */
void lval_print(lval *v) {
  switch (v->type) {
  case LVAL_NUM: printf("%li", v->num); break;
  case LVAL_ERR: printf("%s", v->err); break;
  case LVAL_SYM: printf("%s", v->sym); break;
    case LVAL_SEXPR: lval_expr_print(v, '(', ')'); break;
  }}

/* Print an "lval" followed by a newline */
void lval_println(lval* v) {
  lval_print(v);
  putchar('\n');
}


lval *lval_eval(mpc_ast_t *t);
lval * lval_eval_sexpr(lval*v);

lval * lval_eval_sexpr(lval*v){
}


int main(int argc, char **argv) {

  mpc_parser_t *Number = mpc_new("number");
  mpc_parser_t *Symbol = mpc_new("symbol");
  mpc_parser_t *Sexpr = mpc_new("sexpr");
  mpc_parser_t *Expr = mpc_new("expr");
  mpc_parser_t *Lispy = mpc_new("lispy");

  mpca_lang(MPCA_LANG_DEFAULT,
            "                                                     \
      number   : /-?[0-9]+/ ;                             \
      symbol   : '+' | '-' | '*' | '/' ;                  \
      sexpr    : '(' <expr>* ')' ;                        \
      expr     : <number> | '(' <operator> <expr>+ ')' ;  \
      lispy    : /^/ <operator> <expr>+ /$/ ;             \
    ",
            Number, Symbol, Sexpr, Expr, Lispy);

  puts("Lispy Version 0.0.0.0.4");
  puts("Press Ctrl+c to Exit\n");

  while (1) {
    mpc_result_t r;
    char *input = readline("lispy> ");
    add_history(input);

    if (mpc_parse("<stdin>", input, Lispy, &r)) {
      lval result = eval(r.output);
      lval_println(result);
      mpc_ast_delete(r.output);
    } else {
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }

    free(input);
  }

  mpc_cleanup(5, Number, Symbol, Sexpr, Expr, Lispy);

  return 0;
}
