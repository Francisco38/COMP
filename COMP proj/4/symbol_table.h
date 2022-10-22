#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

typedef struct t1{
	struct t3 *elem;
  	struct t2 *local;
  	struct t1 *next;
}global_table;

typedef struct t2{
	char *name;
 	int params[30];
  	int type;
  	struct t2* next;
  	struct t3* elem;
}local_table;

typedef struct t3{
	char *name;
 	int params[30];
  	int type;
  	int is_param;
  	int is_func;
		int used;
  	struct t3* next;
}table_elem;

struct t3 *insert_elem_global(struct t1 *table, char* name, int p[],int t,int check,int is_func);
struct t2 *insert_local(struct t1 *table, char* name, int p[],int t);
struct t1 *insert_global(struct t1 *table);
struct t3 *insert_elem_local(struct t1 *table,struct t2 *table2, char* name, int p[],int t,int check,int is_func);
void show_table(struct t1 *table);
void show_global_elem(struct t3 *table);
void show_local(struct t2 *table);
void print_type(int t);
table_elem *search_el(struct t1 *global,struct t2 *table, char *str);
table_elem *search_el2(struct t1 *global, char *str);
table_elem *search_el3(struct t2 *table, char *str);
int get_location(struct t1 *global,struct t2 *table, char *str);
#endif
