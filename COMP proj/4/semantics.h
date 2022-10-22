#include "structures.h"
#include "symbol_table.h"

int deftype(struct t1* global_table,no *node);
int check_tree_top(struct t1* global_table,no *node);
void print_error(struct t1* global_table,struct t2* table,no *node);
void error_var(struct t1* global_table,struct t2* table,no *node);
void error_func(struct t1* global_table,struct t2* table,no *node);
void error_funcbody(struct t1* table,struct t2* table2,no *node);
void error_cond(struct t1* global_table,struct t2* table,no *node);
void error_expr(struct t1* global_table,struct t2* table,no *node);
void error_call(struct t1* global_table,struct t2* table,no *node);
void error_if(struct t1* global_table,struct t2* table,no *node);
void error_for(struct t1* global_table,struct t2* table,no *node);
void error_print(struct t1* table,struct t2* table2,no* node);
int check_tree(no *node,struct t1* table);
int check_node(no *node,struct t1* table);
int check_var(no *node,struct t1* table);
int add_func(no *node,struct t1* table);
int check_func(no *node,struct t1* table);
int check_func_body(struct t1* table,struct t2* table2,no * node);
void print_val(char* s);
void printtype(int t);
int check_cond(struct t1* table,struct t2* table2,no* node);
int check_comp(struct t1* table,struct t2* table2,no* node);
int check_math(struct t1* table,struct t2* table2,no* node);
int check_expr(struct t1* table,struct t2* table2,no* node);
int check_call(struct t1* table,struct t2* table2,no* node);
int check_print(struct t1* table,struct t2* table2,no* node);
int check_parseargs(struct t1* table,struct t2* table2,no* node);
int check_if(struct t1* table,struct t2* table2,no* node);
int check_for(struct t1* table,struct t2* table2,no* node);
void error_return(struct t1* global_table,struct t2* table,no *node);
int check_return(struct t1* table,struct t2* table2,no* node);
void error_assign(struct t1* global_table,struct t2* table,no *node);
void error_funcbody_main(struct t1* global_table,struct t2* table,no *node);
void error_arguments(struct t1* global_table,struct t2* table,no *node);
