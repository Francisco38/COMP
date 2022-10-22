#include "semantics.h"
#include "structures.h"
#include "symbol_table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// clang-format off

int deftype(struct t1* global_table,no *node){
	int errorcount = 0;
	no* temp=node;
	if(temp->data.mult->next==NULL){
		errorcount +=check_tree(temp,global_table);
	}
	else{
		errorcount +=check_tree(temp,global_table);
		while(temp->data.mult->next!=NULL){
			errorcount +=check_tree(temp,insert_global(global_table));
			temp=temp->data.mult->next;
		}
		errorcount +=check_tree(temp,insert_global(global_table));
	}
	return errorcount;
}

int check_tree_top(struct t1* global_table,no *node) {
	int errorcount = 0;
	errorcount+=deftype(global_table,node);
	struct t2* table=NULL;
	print_error(global_table,table,node->data.mult->son);
	return errorcount;
}

void print_error(struct t1* global_table,struct t2* table,no *node){
	if(node==NULL){
		return;
	}
	if(strcmp(node->data.mult->name,"VarDecl")==0){
		error_var(global_table,table,node);
	}
	else if(strcmp(node->data.mult->name,"FuncDecl")==0){
		if(table==NULL){
			table=global_table->local;
		}
		else{
			table=table->next;
		}
		error_func(global_table,table,node->data.mult->son);
	}
	print_error(global_table,table,node->data.mult->next);
}

void error_var(struct t1* global_table,struct t2* table,no *node){
	no* id=node->data.mult->son->data.ter->next;
	if(node->error==1){
		printf("Line %d, column %d: Symbol %s already defined\n",id->data.ter->linha,id->data.ter->col,id->data.ter->name);
	}
}

void error_func(struct t1* global_table,struct t2* table,no *node){
	no* id=node->data.mult->son;
	no* func_params;
	switch(id->data.ter->next->type){
		case multi:
			func_params=id->data.ter->next;
			break;
		case terminal:
			func_params=id->data.ter->next->data.ter->next;
			break;
	}
	if(node->error==1){
		printf("Line %d, column %d: Symbol %s already defined\n",id->data.ter->linha,id->data.ter->col,id->data.ter->name);
		return;
	}

	if(func_params->data.mult->son!=NULL){
		no* aux=func_params->data.mult->son;
		while(aux->data.mult->next!=NULL){
			if(aux->data.mult->type==6){
				printf("Line %d, column %d: Symbol %s already defined\n",aux->data.mult->son->data.ter->next->data.ter->linha,aux->data.mult->son->data.ter->next->data.ter->col,aux->data.mult->son->data.ter->next->data.ter->name);
			}
			aux=aux->data.mult->next;
		}
		if(aux->data.mult->type==6){
			printf("Line %d, column %d: Symbol %s already defined\n",aux->data.mult->son->data.ter->next->data.ter->linha,aux->data.mult->son->data.ter->next->data.ter->col,aux->data.mult->son->data.ter->next->data.ter->name);
		}
	}
	error_funcbody_main(global_table,table,node->data.mult->next->data.mult->son);
}

void error_funcbody_main(struct t1* global_table,struct t2* table,no *node){
	if(node==NULL){
		return ;
	}
	switch(node->type){
		case multi:
			if(strcmp(node->data.mult->name,"VarDecl")==0){
				no* id=node->data.mult->son->data.ter->next;
				struct t3* elem;
				if(node->error==1){
					printf("Line %d, column %d: Symbol %s already defined\n",id->data.ter->linha,id->data.ter->col,id->data.ter->name);
				}
				else {
					elem=search_el(global_table,table,id->data.ter->name);
					if(elem->used==1){
						printf("Line %d, column %d: Symbol %s declared but never used\n",id->data.ter->linha,id->data.ter->col,id->data.ter->name);
					}
				}
				error_funcbody_main(global_table,table,node->data.mult->next);
			}
			else if(strcmp(node->data.mult->name,"Or")==0 || strcmp(node->data.mult->name,"And")==0 || strcmp(node->data.mult->name,"Eq")==0 || strcmp(node->data.mult->name,"Ne")==0 || strcmp(node->data.mult->name,"Lt")==0 || strcmp(node->data.mult->name,"Gt")==0 || strcmp(node->data.mult->name,"Le")==0 || strcmp(node->data.mult->name,"Ge")==0 || strcmp(node->data.mult->name,"Add")==0 || strcmp(node->data.mult->name,"Sub")==0 || strcmp(node->data.mult->name,"Mul")==0 || strcmp(node->data.mult->name,"Div")==0 || strcmp(node->data.mult->name,"Mod")==0 || strcmp(node->data.mult->name,"ParseArgs")==0){
				error_cond(global_table,table,node);
				error_funcbody_main(global_table,table,node->data.mult->next);
			}
			else if(strcmp(node->data.mult->name,"Not")==0 || strcmp(node->data.mult->name,"Minus")==0 || strcmp(node->data.mult->name,"Plus")==0 ){
				error_expr(global_table,table,node);
				error_funcbody_main(global_table,table,node->data.mult->next);
			}
			else if(strcmp(node->data.mult->name,"Call")==0){
				error_arguments(global_table,table,node);
				error_call(global_table,table,node);
				error_funcbody_main(global_table,table,node->data.mult->next);
			}
			else if(strcmp(node->data.mult->name,"For")==0){
				error_for(global_table,table,node);
				error_funcbody_main(global_table,table,node->data.mult->next);
			}
			else if(strcmp(node->data.mult->name,"If")==0){
				error_if(global_table,table,node);
				error_funcbody_main(global_table,table,node->data.mult->next);
			}
			else if(strcmp(node->data.mult->name,"Return")==0){
				error_return(global_table,table,node);
				error_funcbody_main(global_table,table,node->data.mult->next);
			}
			else if(strcmp(node->data.mult->name,"Print")==0){
				error_print(global_table,table,node);
				error_funcbody_main(global_table,table,node->data.mult->next);
			}
			else if(strcmp(node->data.mult->name,"Assign")==0){
				error_assign(global_table,table,node);
				error_funcbody_main(global_table,table,node->data.mult->next);
			}
			else{
				error_funcbody_main(global_table,table,node->data.mult->son);
				error_funcbody_main(global_table,table,node->data.mult->next);
			}
			break;
		case terminal:
			if(node->data.ter->tipo==id){
				if(node->data.ter->type==6){
					 printf("Line %d, column %d: Cannot find symbol %s\n",node->data.ter->linha,node->data.ter->col,node->data.ter->name);
				}
			}
			error_funcbody_main(global_table,table,node->data.ter->next);
			break;
	}
}

void error_funcbody(struct t1* global_table,struct t2* table,no *node){
	if(node==NULL){
		return ;
	}
	switch(node->type){
		case multi:
			if(strcmp(node->data.mult->name,"VarDecl")==0){
				no* id=node->data.mult->son->data.ter->next;
				struct t3* elem;
				if(node->error==1){
					printf("Line %d, column %d: Symbol %s already defined\n",id->data.ter->linha,id->data.ter->col,id->data.ter->name);
				}
				else {
					elem=search_el(global_table,table,id->data.ter->name);
					if(elem->used==1){
						printf("Line %d, column %d: Symbol %s declared but never used\n",id->data.ter->linha,id->data.ter->col,id->data.ter->name);
					}
				}
			}
			else if(strcmp(node->data.mult->name,"Or")==0 || strcmp(node->data.mult->name,"And")==0 || strcmp(node->data.mult->name,"Eq")==0 || strcmp(node->data.mult->name,"Ne")==0 || strcmp(node->data.mult->name,"Lt")==0 || strcmp(node->data.mult->name,"Gt")==0 || strcmp(node->data.mult->name,"Le")==0 || strcmp(node->data.mult->name,"Ge")==0 || strcmp(node->data.mult->name,"Add")==0 || strcmp(node->data.mult->name,"Sub")==0 || strcmp(node->data.mult->name,"Mul")==0 || strcmp(node->data.mult->name,"Div")==0 || strcmp(node->data.mult->name,"Mod")==0 || strcmp(node->data.mult->name,"ParseArgs")==0){
				error_cond(global_table,table,node);
			}
			else if(strcmp(node->data.mult->name,"Not")==0 || strcmp(node->data.mult->name,"Minus")==0 || strcmp(node->data.mult->name,"Plus")==0 ){
				error_expr(global_table,table,node);
			}
			else if(strcmp(node->data.mult->name,"Call")==0){
				error_arguments(global_table,table,node);
				error_call(global_table,table,node);
			}
			else if(strcmp(node->data.mult->name,"For")==0){
				error_for(global_table,table,node);
			}
			else if(strcmp(node->data.mult->name,"If")==0){
				error_if(global_table,table,node);
			}
			else if(strcmp(node->data.mult->name,"Return")==0){
				error_return(global_table,table,node);
			}
			else if(strcmp(node->data.mult->name,"Print")==0){
				error_print(global_table,table,node);
			}
			else if(strcmp(node->data.mult->name,"Assign")==0){
				error_assign(global_table,table,node);
			}
			else{
				error_funcbody_main(global_table,table,node);
			}
			break;
		case terminal:
			if(node->data.ter->tipo==id){
				if(node->error==1){
					 printf("Line %d, column %d: Cannot find symbol %s\n",node->data.ter->linha,node->data.ter->col,node->data.ter->name);
				}
			}
			break;
	}
}

void error_return(struct t1* global_table,struct t2* table,no *node){
	no* son=node->data.mult->son;
	error_funcbody(global_table,table,son);
	int type=-1;
	int l,c;
	if(son==NULL){
		type=0;
	}else{
		switch(son->type){
			case multi:
				type=son->data.mult->type;
				l=son->data.mult->linha;
				c=son->data.mult->col;
				break;
			case terminal:
				type=son->data.ter->type;
				l=son->data.ter->linha;
				c=son->data.ter->col;
				break;
		}
	}
	if(node->error==1){
		printf("Line %d, column %d: Incompatible type ",l,c);
		printtype(type);
		printf(" in return statement\n");
	}
}

void error_cond(struct t1* global_table,struct t2* table,no *node){
	no* filho=node->data.mult->son;
	error_funcbody(global_table,table,node->data.mult->son);
	no* filho2=NULL;
	int tipo1=0;
	int type=0;
	switch(filho->type){
		case multi:
			filho2=filho->data.mult->next;
			tipo1=filho->data.mult->type;
			break;
		case terminal:
			filho2=filho->data.ter->next;
			tipo1=filho->data.ter->type;
			break;
	}

	switch(filho2->type){
		case multi:
			type=filho2->data.mult->type;
			break;
		case terminal:
			type=filho2->data.ter->type;
			break;
	}

	if(node->error==1){
		printf("Line %d, column %d: Operator ",node->data.mult->linha,node->data.mult->col);
		print_val(node->data.mult->name);
		printf(" cannot be applied to types ");
		printtype(tipo1);
		printf(", ");
		printtype(type);
		printf("\n");
	}

	error_funcbody(global_table,table,filho2);
}

void error_assign(struct t1* global_table,struct t2* table,no *node){
	no* filho=node->data.mult->son;
	no* filho2;
	int tipo1=0;
	int type=0;
	switch(filho->type){
		case multi:
			filho2=filho->data.mult->next;
			tipo1=filho->data.mult->type;
			break;
		case terminal:
			filho2=filho->data.ter->next;
			tipo1=filho->data.ter->type;
			break;
	}

	switch(filho2->type){
		case multi:
			type=filho2->data.mult->type;
			break;
		case terminal:
			type=filho2->data.ter->type;
			break;
	}
	if(strcmp(filho2->data.mult->name,"Call")==0){
		error_arguments(global_table,table,filho2);
	}
	else{
		error_funcbody(global_table,table,filho);
	}
	if(node->error==1){
		printf("Line %d, column %d: Operator ",node->data.mult->linha,node->data.mult->col);
		print_val(node->data.mult->name);
		printf(" cannot be applied to types ");
		printtype(tipo1);
		printf(", ");
		printtype(type);
		printf("\n");
	}

	if(strcmp(filho2->data.mult->name,"Call")==0){
		error_call(global_table,table,filho2);
		error_funcbody(global_table,table,node->data.mult->son);
	}
	else{
			error_funcbody(global_table,table,filho2);
	}
}

void error_expr(struct t1* global_table,struct t2* table,no *node){
	no* filho=node->data.mult->son;
	int tipo1;
	switch(filho->type){
		case multi:
			tipo1=filho->data.mult->type;
			break;
		case terminal:
			tipo1=filho->data.ter->type;
			break;
	}
	if(node->error==1){
		printf("Line %d, column %d: Operator ",node->data.mult->linha,node->data.mult->col);
		print_val(node->data.mult->name);
		printf(" cannot be applied to type ");
		printtype(tipo1);
		printf("\n");
	}
	error_funcbody(global_table,table,filho);
}

void error_arguments(struct t1* global_table,struct t2* table,no *node){
	no *func_invocation=node->data.mult->son;
	no *aux=func_invocation->data.ter->next;
	while(aux!=NULL){
		switch(aux->type){
			case multi:
				error_funcbody(global_table,table,aux);
				aux=aux->data.mult->next;
				break;
			case terminal:
				error_funcbody(global_table,table,aux);
				aux=aux->data.ter->next;
				break;
		}
	}
}

void error_call(struct t1* global_table,struct t2* table,no *node){
	int params[30];
	no *func_invocation=node->data.mult->son;
	no *aux=func_invocation->data.ter->next;
	int i=0;

	aux=func_invocation->data.ter->next;
	while(aux!=NULL){
		switch(aux->type){
			case multi:
				params[i]=aux->data.mult->type;
				i++;
				aux=aux->data.mult->next;
				break;
			case terminal:
				params[i]=aux->data.ter->type;
				i++;
				aux=aux->data.ter->next;
				break;
		}
	}
	if(node->error==1){
		printf("Line %d, column %d: Cannot find symbol %s(",func_invocation->data.ter->linha,func_invocation->data.ter->col,func_invocation->data.ter->name);
		if(i!=0){
			for(int x=0;x<i-1;x++){
				printtype(params[x]);
				printf(",");
			}
			printtype(params[i-1]);
		}
		printf(")\n");
	}

}

void error_if(struct t1* global_table,struct t2* table,no *node){
	no* son=node->data.mult->son;
	no* son2=NULL;
	int type1;
	int l,c;
	switch(son->type){
		case multi:
			type1=son->data.mult->type;
			l=son->data.mult->linha;
			c=son->data.mult->col;
			son2=son->data.mult->next;
			break;
		case terminal:
			type1=son->data.ter->type;
			son2=son->data.ter->next;
			l=son->data.ter->linha;
			c=son->data.ter->col;
			break;
	}
	if(node->error==1){
		printf("Line %d, column %d: Incompatible type ",l,c);
		printtype(type1);
		printf(" in if statement\n");
	}

	error_funcbody(global_table,table,son);
	error_funcbody(global_table,table,son2);
}

void error_for(struct t1* global_table,struct t2* table,no *node){
	int type1=6;
	no* son=node->data.mult->son;
	no* son2=NULL;
	int l,c;
	switch(son->type){
		case multi:
			if(strcmp(son->data.mult->name,"Block")==0){
				l=node->data.mult->linha;
				c=node->data.mult->col;
			}
			else{
				type1=son->data.mult->type;
				l=son->data.mult->linha;
				c=son->data.mult->col;
			}
			son2=son->data.mult->next;
			break;
		case terminal:
			type1=son->data.ter->type;
			son2=son->data.ter->next;
			l=son->data.ter->linha;
			c=son->data.ter->col;
			break;
	}
	if(node->error==1){
		printf("Line %d, column %d: Incompatible type ",l,c);
		printtype(type1);
		printf(" in for statement\n");
	}

	error_funcbody(global_table,table,son);
	error_funcbody(global_table,table,son2);
}

void error_print(struct t1* table,struct t2* table2,no* node){
	no* son=node->data.mult->son;
	int type=-1;
	int l,c;
	error_funcbody(table,table2,son);
	switch(son->type){
			case multi:
				type=son->data.mult->type;
				l=son->data.mult->linha;
				c=son->data.mult->col;
				break;
				case terminal:
				type=son->data.ter->type;
				l=son->data.ter->linha;
				c=son->data.ter->col;
				break;
		}
	if(node->error==1){
		printf("Line %d, column %d: Incompatible type ",l,c);
		printtype(type);
		printf(" in fmt.Println statement\n");
	}
}

int check_tree(no *node,struct t1* table) {
	int errorcount = 0;
	no* temp=node->data.mult->son;
	if(temp->data.mult->next==NULL){
		errorcount +=check_node(temp,table);
		if(strcmp(temp->data.mult->name,"FuncDecl")==0){
			errorcount +=check_func(temp->data.mult->son,table);
		}
	}
	else{
		while(temp->data.mult->next!=NULL){
			errorcount +=check_node(temp,table);
			temp=temp->data.mult->next;
		}
		errorcount +=check_node(temp,table);
		temp=node->data.mult->son;
		while(temp->data.mult->next!=NULL){
			if(strcmp(temp->data.mult->name,"FuncDecl")==0){
				errorcount +=check_func(temp->data.mult->son,table);
			}
			temp=temp->data.mult->next;
		}
		if(strcmp(temp->data.mult->name,"FuncDecl")==0){
			errorcount +=check_func(temp->data.mult->son,table);
		}
	}

	return errorcount;
}

int check_node(no *node,struct t1* table) {
	int errorcount = 0;
	if(strcmp(node->data.mult->name,"VarDecl")==0){
		errorcount +=check_var(node,table);
	}
	else if(strcmp(node->data.mult->name,"FuncDecl")==0){
		errorcount +=add_func(node->data.mult->son,table);
	}
	return errorcount;
}

int check_var(no *node,struct t1* table){
	int errorcount = 0;
	no* id=node->data.mult->son->data.ter->next;
	if(insert_elem_global(table, id->data.ter->name,NULL,id->data.ter->type,0,0)==NULL){
		node->data.mult->type=6;
		node->error=1;
		errorcount++;
	}
	return errorcount;
}

int add_func(no *node,struct t1* table){
	int errorcount = 0;
	no * func_id=node->data.mult->son;
	struct t3* table3=(struct t3*)malloc(sizeof(struct t3));
	no* func_params;
	switch(func_id->data.ter->next->type){
		case multi:
		func_params=func_id->data.ter->next;
		break;
		case terminal:
		func_params=func_id->data.ter->next->data.ter->next;
		break;
	}
	if(func_params->data.mult->son==NULL){
		table3=insert_elem_global(table, func_id->data.ter->name,NULL,func_id->data.ter->type,0,1);
		if(table3==NULL){
			node->data.mult->type=6;
			node->error=1;
			errorcount++;
			return errorcount;
		}
	}
	else{
		no* aux=func_params->data.mult->son;
		int params[30];
		for(int x=0;x<30;x++){
			params[x]=-1;
		}
		int i=0;
		for(;i<30 && aux->data.mult->next!=NULL;i++){
			params[i]=aux->data.mult->son->data.ter->next->data.ter->type;
			aux=aux->data.mult->next;
		}
		params[i]=aux->data.mult->son->data.ter->next->data.ter->type;
		table3=insert_elem_global(table, func_id->data.ter->name,params,func_id->data.ter->type,0,1);
		if(table3==NULL){
			node->data.mult->type=6;
			node->error=1;
			errorcount++;
		}
	}
	return errorcount;
}

int check_func(no *node,struct t1* table){
	int errorcount = 0;
	no * func_id=node->data.mult->son;
	struct t2* table2=(struct t2*)malloc(sizeof(struct t2));
	no* func_params;
	switch(func_id->data.ter->next->type){
		case multi:
		func_params=func_id->data.ter->next;
		break;
		case terminal:
		func_params=func_id->data.ter->next->data.ter->next;
		break;
	}
	if(func_params->data.mult->son==NULL){
		table2=insert_local(table, func_id->data.ter->name, NULL,func_id->data.ter->type);
	}
	else{
		no* aux=func_params->data.mult->son;
		int params[30];
		for(int x=0;x<30;x++){
			params[x]=-1;
		}
		int i=0;
		for(;i<30 && aux->data.mult->next!=NULL;i++){
			params[i]=aux->data.mult->son->data.ter->next->data.ter->type;
			aux=aux->data.mult->next;
		}
		params[i]=aux->data.mult->son->data.ter->next->data.ter->type;
		table2=insert_local(table, func_id->data.ter->name, params,func_id->data.ter->type);
	}
	insert_elem_local(table,table2,"return",NULL,func_id->data.ter->type,0,0);
	if(func_params->data.mult->son!=NULL){
		no* aux=func_params->data.mult->son;
		while(aux->data.mult->next!=NULL){
			if(insert_elem_local(table,table2, aux->data.mult->son->data.ter->next->data.ter->name,NULL,aux->data.mult->son->data.ter->next->data.ter->type,1,0)==NULL){
				aux->data.mult->type=6;
				errorcount++;
			}
			aux=aux->data.mult->next;
		}
		if(insert_elem_local(table,table2, aux->data.mult->son->data.ter->next->data.ter->name,NULL,aux->data.mult->son->data.ter->next->data.ter->type,1,0)==NULL){
			aux->data.mult->type=6;
			errorcount++;
		}
	}
	errorcount +=check_func_body(table,table2,node->data.mult->next->data.mult->son);
	return errorcount;
}

int check_func_body(struct t1* table,struct t2* table2,no * node){
	int errorcount = 0;
	if(node==NULL){
		return errorcount;
	}
	switch(node->type){
		case multi:
		if(strcmp(node->data.mult->name,"VarDecl")==0){
			no* aux=node->data.mult->son;
			no* id=aux->data.ter->next;
			if(insert_elem_local(table,table2, id->data.ter->name,NULL,id->data.ter->type,0,0)==NULL){
				node->data.mult->type=6;
				node->error=1;
				errorcount++;
			}
			errorcount +=check_func_body(table,table2,node->data.mult->next);
		}
		else if(strcmp(node->data.mult->name,"Or")==0 || strcmp(node->data.mult->name,"And")==0){
			errorcount +=check_cond(table,table2,node);
		}
		else if(strcmp(node->data.mult->name,"Eq")==0 || strcmp(node->data.mult->name,"Ne")==0 || strcmp(node->data.mult->name,"Lt")==0 || strcmp(node->data.mult->name,"Gt")==0 || strcmp(node->data.mult->name,"Le")==0 || strcmp(node->data.mult->name,"Ge")==0 || strcmp(node->data.mult->name,"Assign")==0){
			errorcount +=check_comp(table,table2,node);
			errorcount +=check_func_body(table,table2,node->data.mult->next);
		}
		else if(strcmp(node->data.mult->name,"Add")==0 || strcmp(node->data.mult->name,"Sub")==0 || strcmp(node->data.mult->name,"Mul")==0 || strcmp(node->data.mult->name,"Div")==0 || strcmp(node->data.mult->name,"Mod")==0 ){
			errorcount +=check_math(table,table2,node);
			errorcount +=check_func_body(table,table2,node->data.mult->next);
		}
		else if(strcmp(node->data.mult->name,"Not")==0 || strcmp(node->data.mult->name,"Minus")==0 || strcmp(node->data.mult->name,"Plus")==0 ){
			errorcount +=check_expr(table,table2,node);
			errorcount +=check_func_body(table,table2,node->data.mult->next);
		}
		else if(strcmp(node->data.mult->name,"Call")==0){
			errorcount +=check_call(table,table2,node);
			errorcount +=check_func_body(table,table2,node->data.mult->next);
		}
		else if(strcmp(node->data.mult->name,"ParseArgs")==0){
			errorcount+=check_parseargs(table,table2,node);
			errorcount+=check_func_body(table,table2,node->data.mult->next);
		}
		else if(strcmp(node->data.mult->name,"For")==0){
			errorcount+=check_for(table,table2,node);
			errorcount+=check_func_body(table,table2,node->data.mult->next);
		}
		else if(strcmp(node->data.mult->name,"If")==0){
			errorcount+=check_if(table,table2,node);
			errorcount+=check_func_body(table,table2,node->data.mult->next);
		}
		else if(strcmp(node->data.mult->name,"Return")==0){
			errorcount+=check_return(table,table2,node);
			errorcount+=check_func_body(table,table2,node->data.mult->next);
		}
		else if(strcmp(node->data.mult->name,"Print")==0){
			errorcount+=check_print(table,table2,node);
			errorcount+=check_func_body(table,table2,node->data.mult->next);
		}
		else{
			errorcount +=check_func_body(table,table2,node->data.mult->son);
			errorcount +=check_func_body(table,table2,node->data.mult->next);
		}
		break;
		case terminal:
		if(node->data.ter->tipo==id){

			struct t3* table3=search_el(table,table2,node->data.ter->name);
			if(table3==NULL){
				node->data.ter->type=6;
				node->error=1;
				errorcount++;
			}
			else if(table3->is_func==1){
				node->data.ter->type=6;
				node->error=1;
				errorcount++;
			}
			else{

				node->data.ter->type=table3->type;
			}
		}
		break;
	}
	return errorcount;
}

void print_val(char* s){
	if(strcmp(s,"Or")==0){
		printf("||");
	}
	else if(strcmp(s,"And")==0){
		printf("&&");
	}
	else if(strcmp(s,"Eq")==0){
		printf("==");
	}
	else if(strcmp(s,"Ne")==0){
		printf("!=");
	}
	else if(strcmp(s,"Lt")==0){
		printf("<");
	}
	else if(strcmp(s,"Gt")==0){
		printf(">");
	}
	else if(strcmp(s,"Le")==0){
		printf("<=");
	}
	else if(strcmp(s,"Ge")==0){
		printf(">=");
	}
	else if(strcmp(s,"Assign")==0){
		printf("=");
	}
	else if(strcmp(s,"Sub")==0 || strcmp(s,"Minus")==0){
		printf("-");
	}
	else if(strcmp(s,"Add")==0 || strcmp(s,"Plus")==0){
		printf("+");
	}
	else if(strcmp(s,"Mul")==0){
		printf("*");
	}
	else if(strcmp(s,"Div")==0){
		printf("/");
	}
	else if(strcmp(s,"Mod")==0){
		printf("%%");
	}
	else if(strcmp(s,"Not")==0){
		printf("!");
	}
	else if(strcmp(s,"ParseArgs")==0){
		printf("strconv.Atoi");
	}
}

void printtype(int t){
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

int check_cond(struct t1* table,struct t2* table2,no* node){
	int errorcount = 0;
	int error=0;
	no* filho=node->data.mult->son;
	switch(filho->type){
		case multi:
		errorcount +=check_func_body(table,table2,filho);

		if(filho->data.mult->type!=2){
			error=1;
		}
		filho=filho->data.mult->next;
		break;
		case terminal:
		errorcount +=check_func_body(table,table2,filho);

		if(filho->data.ter->type!=2){
			error=1;
		}
		filho=filho->data.ter->next;
		break;
	}
	switch(filho->type){
		case multi:
		errorcount +=check_func_body(table,table2,filho);

		if(filho->data.mult->type!=2){
			error=1;
		}
		filho=node->data.mult->next;
		break;
		case terminal:
		errorcount +=check_func_body(table,table2,filho);

		if(filho->data.ter->type!=2){
			error=1;
		}
		filho=node->data.ter->next;
		break;
	}

	if(error==1){
		errorcount++;
		node->error=1;
	}
	return errorcount;
}

int check_comp(struct t1* table,struct t2* table2,no* node){
	int errorcount = 0;
	int type1=-1;
	int type2=-1;
	no* filho=node->data.mult->son;
	no* filho2=NULL;
	switch(filho->type){
		case multi:
		errorcount +=check_func_body(table,table2,filho);

		type1=filho->data.mult->type;
		filho2=filho->data.mult->next;
		break;
		case terminal:
		errorcount +=check_func_body(table,table2,filho);

		type1=filho->data.ter->type;
		filho2=filho->data.ter->next;
		break;
	}
	switch(filho2->type){
		case multi:
		errorcount +=check_func_body(table,table2,filho2);
		type2=filho2->data.mult->type;
		break;
		case terminal:
			errorcount +=check_func_body(table,table2,filho2);
			type2=filho2->data.ter->type;
		break;
	}
	if(type1!=type2 || type1==6){
			errorcount++;
			node->error=1;
	}
	else if(strcmp(node->data.mult->name,"Assign")==0){
		node->data.mult->type=type1;
	}
	else{
		if(type1==2){
			if(!(strcmp(node->data.mult->name,"Eq")==0 || strcmp(node->data.mult->name,"Ne")==0)){
				errorcount++;
				node->error=1;
			}
		}
	}
	return errorcount;
}

int check_math(struct t1* table,struct t2* table2,no* node){
	int errorcount = 0;
	int type1=-1;
	int type2=-1;
	no* filho=node->data.mult->son;
	switch(filho->type){
		case multi:
		errorcount +=check_func_body(table,table2,filho);

		type1=filho->data.mult->type;
		filho=filho->data.mult->next;
		break;
		case terminal:
		errorcount +=check_func_body(table,table2,filho);

		type1=filho->data.ter->type;
		filho=filho->data.ter->next;
		break;
	}

	switch(filho->type){
		case multi:
		errorcount +=check_func_body(table,table2,filho);

		type2=filho->data.mult->type;
		filho=filho->data.mult->next;
		break;
		case terminal:
		errorcount +=check_func_body(table,table2,filho);

		type2=filho->data.ter->type;
		filho=filho->data.ter->next;
		break;
	}

	if(type1!=type2 || type1==2){
		errorcount++;
		node->data.mult->type=6;
		node->error=1;
	}
	else{
		node->data.mult->type=type1;
	}
	return errorcount;
}

int check_expr(struct t1* table,struct t2* table2,no* node){
	int errorcount = 0;
	if(strcmp(node->data.mult->name,"Not")==0){
		no* son=node->data.mult->son;
		switch(son->type){
			case multi:
			errorcount +=check_func_body(table,table2,son);

			if(son->data.mult->type!=2){
				node->data.mult->type=2;
				node->error=1;
				errorcount++;
			}
			break;
			case terminal:
			errorcount +=check_func_body(table,table2,son);

			if(son->data.ter->type!=2){
				node->data.mult->type=2;
				node->error=1;
				errorcount++;
			}
			break;
		}
	}
	else{
		no* son=node->data.mult->son;
		switch(son->type){
			case multi:
			errorcount +=check_func_body(table,table2,son);
			if(son->data.mult->type==1 || son->data.mult->type==3){
				node->data.mult->type=son->data.mult->type;
			}
			else{
				node->data.mult->type=6;
				node->error=1;
				errorcount++;
			}
			break;
			case terminal:
			errorcount +=check_func_body(table,table2,son);
			if(son->data.ter->type==1 || son->data.ter->type==3){
				node->data.mult->type=son->data.ter->type;
			}
			else{
				node->data.mult->type=6;
				node->error=1;
				errorcount++;
			}
			break;
		}
	}
	return errorcount;
}

int check_call(struct t1* table,struct t2* table2,no* node){
	int errorcount = 0;
	int error=0;
	int params[30];
	no *func_invocation=node->data.mult->son;
	struct t3* func_entry=search_el(table,table2, func_invocation->data.ter->name);
	no *aux=func_invocation->data.ter->next;
	int i=0;
	while(aux!=NULL){
		switch(aux->type){
			case multi:
			errorcount +=check_func_body(table,table2,aux);
			params[i]=aux->data.mult->type;
			i++;
			aux=aux->data.mult->next;
			break;
			case terminal:
			errorcount +=check_func_body(table,table2,aux);

			params[i]=aux->data.ter->type;
			i++;
			aux=aux->data.ter->next;
			break;
		}
	}
	for(int d=i;d<30;d++){
		params[d]=-1;
	}
	if(func_entry==NULL){
		node->data.mult->type=6;
		node->error=1;
		error=1;
		errorcount++;
	}
	if(error==0){
		for(int x=0;x<30;x++){
			if(func_entry->params[x]!=params[x]){
				node->data.mult->type=6;
				node->error=1;
				error=1;
				errorcount++;
				break;
			}
		}
	}

	if(error==0){
		node->data.mult->type=func_entry->type;
		for(int v=0;v<30;v++){
			func_invocation->data.ter->params[v]=params[v];
		}
	}
	else{
		node->data.mult->type=6;
		node->error=1;
		func_invocation->data.ter->type=6;
	}
	return errorcount;
}

int check_parseargs(struct t1* table,struct t2* table2,no* node){
	int errorcount=0;
	no* son=node->data.mult->son;
	errorcount+=check_func_body(table,table2,son);
	int type1=son->data.ter->type;
	int type2=-1;
	son=son->data.ter->next;
	errorcount+=check_func_body(table,table2,son);
		switch(son->type){
			case multi:
			type2=son->data.mult->type;
			break;
			case terminal:
			type2=son->data.ter->type;
			break;
	}
	if(type1!=1 || type2!=1){
		node->data.mult->type=6;
		node->error=1;
		errorcount++;
	}
	return errorcount;
}

int check_if(struct t1* table,struct t2* table2,no* node){
	int errorcount=0;
	no* son=node->data.mult->son;
	no* son2=NULL;
	errorcount +=check_func_body(table,table2,son);

	int type1;
	switch(son->type){
		case multi:
		type1=son->data.mult->type;
		son2=son->data.mult->next;
		break;
		case terminal:
		type1=son->data.ter->type;
		son2=son->data.ter->next;
		break;
	}
	if(type1!=2){
		node->data.mult->type=6;
		node->error=1;
		errorcount++;
	}
	errorcount +=check_func_body(table,table2,son2);
	return errorcount;
}

int check_for(struct t1* table,struct t2* table2,no* node){
	int errorcount=0;
	int type1=6;
	no* son=node->data.mult->son;
	no* son2;
	switch(son->type){
		case multi:
		if(strcmp(son->data.mult->name,"Block")==0){
			errorcount +=check_func_body(table,table2,son);
		}
		else{
			errorcount +=check_func_body(table,table2,son);
			type1=son->data.mult->type;
		}
		son2=son->data.mult->next;
		errorcount +=check_func_body(table,table2,son2);
		break;
		case terminal:
			errorcount +=check_func_body(table,table2,son);
			type1=son->data.ter->type;
			son2=son->data.ter->next;
			errorcount +=check_func_body(table,table2,son2);
			break;
	}
	if(type1!=2 && type1!=6){
		node->data.mult->type=6;
		node->error=1;
		errorcount++;
	}
	return errorcount;
}

int check_return(struct t1* table,struct t2* table2,no* node){
	int errorcount=0;
	struct t3* table3=search_el(table,table2,"return");
	no* son=node->data.mult->son;
	int type=-1;
	errorcount+=check_func_body(table,table2,son);
	if(son==NULL){
		type=0;
	}
	else{switch(son->type){
			case multi:
				type=son->data.mult->type;
				break;
				case terminal:
				type=son->data.ter->type;
				break;
		}
	}
	if(type!=table3->type){
		node->data.mult->type=6;
		node->error=1;
		errorcount++;
	}
	return errorcount;
}

int check_print(struct t1* table,struct t2* table2,no* node){
	int errorcount=0;
	no* son=node->data.mult->son;
	int type=-1;
	errorcount+=check_func_body(table,table2,son);
	switch(son->type){
			case multi:
				type=son->data.mult->type;
				break;
				case terminal:
				type=son->data.ter->type;
				break;
		}
	if(type==6){
		node->data.mult->type=6;
		node->error=1;
		errorcount++;
	}
	return errorcount;
}
