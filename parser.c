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

long eval(mpc_ast_t *t);
long eval_op(long x, char *op, long y);

int main(int argc, char **argv) {
  mpc_parser_t *Number = mpc_new("number");
  mpc_parser_t *Operator = mpc_new("operator");
  mpc_parser_t *Expr = mpc_new("expr");
  mpc_parser_t *Lispy = mpc_new("lispy");
  mpc_result_t r;
  /* Define them with the following Language */
  mpca_lang(MPCA_LANG_DEFAULT,
            "                                                     \
            number   : /-?[0-9]+/ ;                             \
            operator : '+' | '-' | '*' | '/' ;                  \
            expr     : <number> | '(' <operator> <expr>+ ')' ;  \
            lispy    : /^/ <operator> <expr>+ /$/ ;             \
            ",
            Number, Operator, Expr, Lispy);

  puts("Lispy Version 0.0.0.0.1");
  puts("Press Ctrl+c to Exit\n");
  while (1) {
    char *input = readline("lispy~> ");
    add_history(input);
    if (mpc_parse("<stdin>", input, Lispy, &r)) {
      printf("%li\n", eval(r.output));
      mpc_ast_delete(r.output);
    } else {
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }

    free(input);
  }
  mpc_cleanup(4, Number, Operator, Expr, Lispy);
  return 0;
}

long eval(mpc_ast_t *t) {
  if (strstr((*t).tag, "number")) {
    return (atoi((*t).contents));
  }
  char *op = (*t).children[1]->contents;
  long x = eval((*t).children[2]);
  int i = 3;

  while (strstr(t->children[i]->tag, "expr")) {
    x = eval_op(x, op, eval(t->children[i]));
    i++;
  }

  return x;
}

long eval_op(long x, char *op, long y) {
  if (strcmp(op, "+") == 0) {
    return x + y;
  }
  if (strcmp(op, "-") == 0) {
    return x - y;
  }
  if (strcmp(op, "*") == 0) {
    return x * y;
  }
  if (strcmp(op, "/") == 0) {
    return x / y;
  }
  return 0;
}
