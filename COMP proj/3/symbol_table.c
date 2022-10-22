#include "symbol_table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct t3 *insert_elem_global(struct t1 *table, char* name, int p[],int t,int check,int is_func){
	if(search_el2(table,name)!=NULL){
		return NULL;
	}
	struct t3 *new_elem = (struct t3*)malloc(sizeof(struct t3));
	new_elem->name=name;
	if(p!=NULL){
		for(int i=0;i<30;i++){
			new_elem->params[i]=p[i];
		}
	}
	else{
		for(int i=0;i<30;i++){
			new_elem->params[i]=-1;
		}
	}
	new_elem->type=t;
	new_elem->is_param=check;
	new_elem->is_func=is_func;
	struct t1* aux=table;
	struct t3* aux2=aux->elem;
	if(aux->elem==NULL){
		aux->elem=new_elem;
	}
	else{
		while(aux2->next!=NULL){
			aux2=aux2->next;
		}
		aux2->next=new_elem;
	}
	return new_elem;
}

struct t2 *insert_local(struct t1 *table, char* name, int p[],int t){
	struct t2 *new_table = (struct t2 *)malloc(sizeof(struct t2));
	new_table->name=name;
	if(p!=NULL){
		for(int i=0;i<30;i++){
			new_table->params[i]=p[i];
		}
	}
	else{
		for(int i=0;i<30;i++){
			new_table->params[i]=-1;
		}
	}
	new_table->type=t;

	struct t1* aux=table;
	struct t2* aux2=aux->local;

	if(aux->local==NULL){
		aux->local=new_table;
	}
	else{
		while(aux2->next!=NULL){
			aux2=aux2->next;
		}
		aux2->next=new_table;
	}
	return new_table;
}

struct t1 *insert_global(struct t1 *table){
	struct t1* table2 = (struct t1*)malloc(sizeof(struct t1));

	table2->local=NULL;
	table2->elem=NULL;
	table2->next=NULL;
	if(table==NULL){
		return table2;
	}
	else{
		struct t1* aux=table;

		if(aux->next==NULL){
			aux->next=table2;
		}
		else{
			while(aux->next!=NULL){
				aux=aux->next;
			}
			aux->next=table2;
		}
	}
	return table2;
}

struct t3 *insert_elem_local(struct t1 *table,struct t2 *table2, char* name, int p[],int t,int check,int is_func){
	if(search_el3(table2,name)!=NULL){
		return NULL;
	}
	struct t3 *new_elem = (struct t3*)malloc(sizeof(struct t3));
	new_elem->name=name;
	if(p!=NULL){
		for(int i=0;i<30;i++){
			new_elem->params[i]=p[i];
		}
	}
	else{
		for(int i=0;i<30;i++){
			new_elem->params[i]=-1;
		}
	}
	new_elem->type=t;
	new_elem->is_param=check;
	new_elem->is_func=is_func;

	struct t2* aux=table2;
	struct t3* aux2=aux->elem;
	if(aux->elem==NULL){
		aux->elem=new_elem;
	}
	else{
		while(aux2->next!=NULL){
			aux2=aux2->next;
		}
		aux2->next=new_elem;
	}
	return new_elem;
}

void show_table(struct t1 *table) {
	struct t1* aux=table;

	printf("===== Global Symbol Table =====\n");
	if(aux->elem!=NULL){
		show_global_elem(aux->elem);
		printf("\n");
	}
	if(aux->local!=NULL){
		show_local(aux->local);
		printf("\n");
	}
	if(aux->next!=NULL){
		show_table(aux->next);
		printf("\n");
	}
}

void show_global_elem(struct t3 *table){
	printf("%s\t",table->name);
	if(table->is_func==1){
		printf("(");
		if(table->params[0]!=-1){
			print_type(table->params[0]);
			for(int i=1;table->params[i]!=-1;i++){
				printf(",");
				print_type(table->params[i]);
			}
		}
		printf(")");
	}
	printf("\t");
	print_type(table->type);
	if(table->is_param==1){
		printf("\tparam");
	}
	printf("\n");
	if(table->next!=NULL){
		show_global_elem(table->next);
	}
}

void print_type(int t){
	switch(t){
		case 0:
			printf("none");
			break;
		case 1:
			printf("int");
			break;
		case 2:
			printf("bool");
			break;
		case 3:
			printf("float32");
			break;
		case 4:
			printf("string");
			break;
		case 5:
			printf("random");
			break;
	 	case 6:
			printf("undef");
			break;
	}
}

void show_local(struct t2 *table){
	printf("===== Function %s",table->name);
	printf("(");
	if(table->params[0]!=-1){
		print_type(table->params[0]);
		for(int i=1;table->params[i]!=-1;i++){
			printf(",");
			print_type(table->params[i]);
		}
	}
	printf(") ");
	printf("Symbol Table =====\n");
	if(table->elem!=NULL){
		show_global_elem(table->elem);
	}
	if(table->next!=NULL){
		printf("\n");
		show_local(table->next);
	}
}

table_elem *search_el2(struct t1 *global, char *str) {
  	struct t1 *aux=global;

  	if(aux->elem!=NULL){
  		if(strcmp(aux->elem->name, str)==0){
  			return aux->elem;
  		}
  		else{
  			struct t3 *aux3=aux->elem;
  			while(aux3->next!=NULL && strcmp(aux3->name, str) != 0){
  				aux3=aux3->next;
  			}
  			if(strcmp(aux3->name, str) == 0){
					aux3->used++;
  				return aux3;
  			}
  		}
  	}
  	return NULL;
}

table_elem *search_el(struct t1 *global,struct t2 *table, char *str) {
  	struct t1 *aux=global;
  	struct t2 *aux2=table;

  	if(aux2->elem!=NULL){
  		if(strcmp(aux2->elem->name, str)==0){
  			return aux2->elem;
  		}
  		else{
  			struct t3 *aux3=aux2->elem;
  			while(aux3->next!=NULL && strcmp(aux3->name, str) != 0){
  				aux3=aux3->next;
  			}
  			if(strcmp(aux3->name, str) == 0){
					aux3->used++;
  				return aux3;
  			}
  		}
  	}

  	if(aux->elem!=NULL){
  		if(strcmp(aux->elem->name, str)==0){
  			return aux->elem;
  		}
  		else{
  			struct t3 *aux3=aux->elem;
  			while(aux3->next!=NULL && strcmp(aux3->name, str) != 0){
  				aux3=aux3->next;
  			}
  			if(strcmp(aux3->name, str) == 0){
					aux3->used++;
  				return aux3;
  			}
  		}
  	}
  	return NULL;
}

table_elem *search_el3(struct t2 *table, char *str){
	struct t2 *aux2=table;

	if(aux2->elem!=NULL){
  		if(strcmp(aux2->elem->name, str)==0){
  			return aux2->elem;
  		}
  		else{
  			struct t3 *aux3=aux2->elem;
  			while(aux3->next!=NULL && strcmp(aux3->name, str) != 0){
  				aux3=aux3->next;
  			}
  			if(strcmp(aux3->name, str) == 0){
					aux3->used++;
  				return aux3;
  			}
  		}
  	}

  	return NULL;
}
