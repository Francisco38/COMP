#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "structures.h"
#include "functions.h"

no* getNext(no* n){
	switch(n->type){
		case multi:
			return n->data.mult->next;
			break;
		case terminal:
			return n->data.ter->next;
			break;
	}
	return NULL;
}

int count_next(no* n){
	no* temp=n;
	int count=1;
	while(getNext(temp)!=NULL){
		count++;
		temp=getNext(temp);
	}
	return count;
}

no* create_id(char* type){
	char* var=(char *) malloc(strlen(type) + 5);
	sprintf(var, "Id(%s)", type);

  	no* node;
  	no_terminal* info;
  	
  	node = malloc(sizeof(no));
  	info = malloc(sizeof(no_terminal));
  	
  	info->type=var;
  	info->next=NULL;
	node->type = terminal;
  	node->data.ter=info;
	return node;
}


no* create_char(char* type){
	char* var=(char *) malloc(strlen(type) + 15);
	sprintf(var, "StrLit(\"%s\")", type);

  	no* node;
  	no_terminal* info;
  	
  	node = malloc(sizeof(no));
  	info = malloc(sizeof(no_terminal));
  	
  	info->type=var;
  	info->next=NULL;
	node->type = terminal;
  	node->data.ter=info;
	return node;
}


no* create_reallit(char* type){
	char* var=(char *) malloc(strlen(type) + 10);
	sprintf(var, "RealLit(%s)", type);

  	no* node;
  	no_terminal* info;
  	
  	node = malloc(sizeof(no));
  	info = malloc(sizeof(no_terminal));
  	
  	info->type=var;
  	info->next=NULL;
	node->type = terminal;
  	node->data.ter=info;
	return node;
}

no* create_intlit(char* type){
	char* var=(char *) malloc(strlen(type) + 10);
	sprintf(var, "IntLit(%s)", type);

  	no* node;
  	no_terminal* info;
  	
  	node = malloc(sizeof(no));
  	info = malloc(sizeof(no_terminal));
  	
  	info->type=var;
  	info->next=NULL;
	node->type = terminal;
  	node->data.ter=info;
	return node;
}

no* create_type(char* type){
  	no* node;
  	no_terminal* info;
  	
  	node = malloc(sizeof(no));
  	info = malloc(sizeof(no_terminal));
  	
  	info->type=type;
  	info->next=NULL;
	node->type = terminal;
  	node->data.ter=info;
	return node;
}

no* create_multi(char* type){
  	no* node;
  	no_multi* info;
  	
  	node = malloc(sizeof(no));
  	info = malloc(sizeof(no_multi));
  	
  	info->type=type;
  	info->son=NULL;
  	info->next=NULL;
	node->type = multi;
  	node->data.mult=info;
	return node;
}

void add_son_ini(no* n, no* node){
  	if (n==NULL || node==NULL){
    	return;
 	}
	if(n->type==multi){
		no*aux=n->data.mult->son;
		switch(node->type){
			case multi:
				node->data.mult->next=aux;
				break;
			case terminal:
				node->data.ter->next=aux;
				break;
		}
    	n->data.mult->son = node;
    }
    else{
    	printf("Erro: não e possivel adicionar finhos num no termainal\n");
    }
}


void add_son_end(no* n, no* node){
  	if (n==NULL || node==NULL){
    	return;
 	}
	if(n->type==multi){
		if(n->data.mult->son==NULL){
      		n->data.mult->son=node;
    	}
    	else{
      		no*aux=n->data.mult->son;
      		add_sibling(aux,node);
    	}
    }
    else{
    	printf("Erro: não e possivel adicionar finhos num no termainal\n");
    }
}

void add_sibling(no* n, no* node){
  	if (n==NULL || node==NULL){
    	return;
 	}
	no*aux=n;
   	if(getNext(aux)==NULL){
   		switch(aux->type){
			case multi:
				aux->data.mult->next=node;
				break;
			case terminal:
				aux->data.ter->next=node;
				break;
		}
    }
    else{
     	while(getNext(aux)!=NULL){
     		aux=getNext(aux);
      	}
      	switch(aux->type){
			case multi:
				aux->data.mult->next=node;
				break;
			case terminal:
				aux->data.ter->next=node;
				break;
		}
    }
}


void showTree(int level, no* n){
  	if(n==NULL){
    	return;
  	}
  	else {
    	int m=0;
    	while(m<level){
      		printf("..");
      		m++;
    	}

   	 	switch(n->type){
			case multi:
				printf("%s\n",n->data.mult->type);
				showTree(level+1, n->data.mult->son);
				showTree(level, n->data.mult->next);
				break;
			case terminal:
				printf("%s\n",n->data.ter->type);
				showTree(level, n->data.ter->next);
				break;
		}
  	}
}

void freeMemory(no* n){
  	if (n == NULL){
    	return;
 	}
	switch(n->type){
			case multi:
				if (n->data.mult->son != NULL){
    				freeMemory(n->data.mult->son);
  				}
  				if (n->data.mult->next != NULL){
    				freeMemory(n->data.mult->next);
  				}
				break;
			case terminal:
				if (n->data.ter->next != NULL){
    				freeMemory(n->data.ter->next);
  				}
				break;
	}
  	free(n);
}

void fixVarSpec(no* n,no* t){
	no* temp =n;
  	char* tipo=t->data.ter->type;
   	if(getNext(temp)== NULL){
      no* node=create_type(tipo);
      add_son_ini(n, node);
    }
    else{
      	while(getNext(temp)!=NULL){
      		no* node=create_type(tipo);
      		add_son_ini(temp, node);
        	temp=getNext(temp);
      	}
      	no* node=create_type(tipo);
      	add_son_ini(temp, node);
    }
}
