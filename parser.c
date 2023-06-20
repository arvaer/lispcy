#include <stdio.h>
#include <stdlib.h>
#include "mpc.h"

#ifdef _WIN32
#include <string.h>
static char input[2048];
char* readline (const char *prompt) {
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


void add_history(char *in){}

#else
#include <readline/readline.h>
#include <readline/history.h>

#endif /*_WIN32 */





int main(int argc, char** argv){
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
    while(1){
        char* input = readline("lispy~> ");
        add_history(input);
        if (mpc_parse("<stdin>", input, Lispy, &r)){
            mpc_ast_print(r.output);
            mpc_ast_delete(r.output);
        }
        else{
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
        }

        free(input);
    }
    mpc_cleanup(4, Number, Operator, Expr, Lispy);
    return 0;

}
