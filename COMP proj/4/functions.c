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

void add_type_id(no_terminal* n,char* tipo){
	if(tipo==NULL){
  		n->type=0;
  		return;
  	}
	if(strcmp(tipo,"Int")==0){
  		n->type=1;
  	}
  	else if(strcmp(tipo,"Bool")==0){
  		n->type=2;
  	}
  	else if(strcmp(tipo,"Float32")==0){
  		n->type=3;
  	}
  	else if(strcmp(tipo,"String")==0){
  		n->type=4;
  	}
}

no* create_id(char* name,no* n,int l,int c){
	char* tipo;
	if(n!=NULL){
		tipo=n->data.ter->name;
	}else{
		tipo=NULL;
	}

  	no* node;
  	no_terminal* info;

  	node = malloc(sizeof(no));
  	info = malloc(sizeof(no_terminal));
  	for(int i=0;i<30;i++){
  		info->params[i]=-1;
  	}
		info->stored=0;
  	info->name=name;
  	info->tipo=id;
  	info->linha=l;
  	info->col=c;
  	info->next=NULL;
	node->type = terminal;
	node->error=0;
  	node->data.ter=info;
  	add_type_id(info,tipo);
	return node;
}

no* create_id_notype(char* name,int l,int c){
  	no* node;
  	no_terminal* info;

  	node = malloc(sizeof(no));
  	info = malloc(sizeof(no_terminal));
  	for(int i=0;i<30;i++){
  		info->params[i]=-1;
  	}
		info->stored=0;
  	info->name=name;
  	info->tipo=id;
  	info->type=5;
  	info->linha=l;
  	info->col=c;
  	info->next=NULL;
	node->type = terminal;
	node->error=0;
  	node->data.ter=info;
	return node;
}

no* create_char(char* name,int l,int c){
  	no* node;
  	no_terminal* info;

  	node = malloc(sizeof(no));
  	info = malloc(sizeof(no_terminal));
  	for(int i=0;i<30;i++){
  		info->params[i]=-1;
  	}
		info->stored=0;
  	info->name=name;
  	info->tipo=str;
  	info->type=4;
  	info->linha=l;
  	info->col=c;
  	info->next=NULL;
	node->type = terminal;
	node->error=0;
  	node->data.ter=info;
	return node;
}


no* create_reallit(char* name,int l,int c){
	no* node;
  	no_terminal* info;

  	node = malloc(sizeof(no));
  	info = malloc(sizeof(no_terminal));
  	for(int i=0;i<30;i++){
  		info->params[i]=-1;
  	}
		info->stored=0;
  	info->name=name;
  	info->tipo=reallit;
  	info->type=3;
  	info->linha=l;
  	info->col=c;
  	info->next=NULL;
	node->type = terminal;
	node->error=0;
  	node->data.ter=info;
	return node;
}

no* create_intlit(char* name,int l,int c){
	no* node;
  	no_terminal* info;

  	node = malloc(sizeof(no));
  	info = malloc(sizeof(no_terminal));

  	for(int i=0;i<30;i++){
  		info->params[i]=-1;
  	}
		info->stored=0;
  	info->name=name;
  	info->tipo=intlit;
  	info->type=1;
  	info->linha=l;
  	info->col=c;
  	info->next=NULL;
	node->type = terminal;
	node->error=0;
  	node->data.ter=info;
	return node;
}

no* create_type(char* name,int l,int c){
	no* node;
  	no_terminal* info;

  	node = malloc(sizeof(no));
  	info = malloc(sizeof(no_terminal));
  	for(int i=0;i<30;i++){
  		info->params[i]=-1;
  	}
		info->stored=0;
  	info->name=name;
  	info->tipo=type;
  	info->type=0;
  	info->linha=l;
  	info->col=c;
  	info->next=NULL;
	node->type = terminal;
	node->error=0;
  	node->data.ter=info;
	return node;
}

no* create_multi(char* name,int t,int l,int c){
  	no* node;
  	no_multi* info;

  	node = malloc(sizeof(no));
  	info = malloc(sizeof(no_multi));
  	info->name=name;
  	info->type=t;
  	info->linha=l;
  	info->col=c;
  	info->son=NULL;
  	info->next=NULL;
	node->type = multi;
	node->error=0;
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

void showTerm(no* n){
	switch(n->data.ter->tipo){
			case id:
				printf("Id(%s)",n->data.ter->name);
				break;
			case str:
				printf("StrLit(\"%s\")",n->data.ter->name);
				break;
			case intlit:
				printf("IntLit(%s)",n->data.ter->name);
				break;
			case reallit:
				printf("RealLit(%s)",n->data.ter->name);
				break;
			case type:
				printf("%s",n->data.ter->name);
				break;
	}
}

void printType(int t){
	switch(t){
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

void printCall(int level,no* node){
    printf("%s",node->data.mult->name);
    if(node->data.mult->type!=0){
		printf(" - ");
		printType(node->data.mult->type);

	}
    printf("\n");
    int m=0;
    while(m<level+1){
      	printf("..");
      	m++;
    }
    no* func=node->data.mult->son;
    printf("Id(%s) - (",func->data.ter->name);
    if(func->data.ter->params[0]!=-1){
    	printType(func->data.ter->params[0]);
    	for(int i=1;i<30 && func->data.ter->params[i]!=-1;i++){
    		printf(",");
    		printType(func->data.ter->params[i]);
    	}
    }
    printf(")\n");
    showTreeInfo(level+1,func->data.ter->next);
}

void printHeader(int level,no* n){
	 if(n==NULL){
    	return;
  	}
	int m=0;
    while(m<level){
      	printf("..");
      	m++;
    }
	switch(n->type){
		case multi:
			printf("%s\n",n->data.mult->name);
			printHeader(level+1, n->data.mult->son);
			printHeader(level, n->data.mult->next);
			break;
		case terminal:
			showTerm(n);
			printf("\n");
			printHeader(level, n->data.ter->next);
			break;
	}
}

void showTreeInfo(int level, no* n){
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
				if(strcmp(n->data.mult->name,"Call")==0){
					printCall(level,n);
					showTreeInfo(level, n->data.mult->next);
				}
				else if(strcmp(n->data.mult->name,"FuncHeader")==0){
					printf("FuncHeader\n");
					printHeader(level+1,n->data.mult->son);
					showTreeInfo(level, n->data.mult->next);
				}
				else if(strcmp(n->data.mult->name,"VarDecl")==0){
					printf("VarDecl\n");
					printHeader(level+1,n->data.mult->son);
					showTreeInfo(level, n->data.mult->next);
				}
				else{
					printf("%s",n->data.mult->name);
					if(n->data.mult->type!=0){
						printf(" - ");
						printType(n->data.mult->type);
					}
    				printf("\n");
					showTreeInfo(level+1, n->data.mult->son);
					showTreeInfo(level, n->data.mult->next);
				}
				break;
			case terminal:
				showTerm(n);
				if(n->data.ter->type!=0){
					printf(" - ");
					printType(n->data.ter->type);
				}
    			printf("\n");
				showTreeInfo(level, n->data.ter->next);
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
				printf("%s\n",n->data.mult->name);
				showTree(level+1, n->data.mult->son);
				showTree(level, n->data.mult->next);
				break;
			case terminal:
				showTerm(n);
				printf("\n");
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
  	char* tipo=t->data.ter->name;
  	int linha=t->data.ter->linha;
  	int col=t->data.ter->col;
   	if(getNext(temp)== NULL){
      no* node=create_type(tipo,linha,col);
      add_type_id(n->data.mult->son->data.ter, tipo);
      add_son_ini(n, node);
    }
    else{
      	while(getNext(temp)!=NULL){
      		no* node=create_type(tipo,linha,col);
      		add_type_id(temp->data.mult->son->data.ter, tipo);
      		add_son_ini(temp, node);
        	temp=getNext(temp);
      	}
      	no* node=create_type(tipo,linha,col);
      	add_type_id(temp->data.mult->son->data.ter, tipo);
      	add_son_ini(temp, node);
    }
}
