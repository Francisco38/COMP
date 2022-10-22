#include <stdio.h>
#include <stdlib.h>
#include "structures.h"

//return o porximo no
no* getNext(no* n);

//returns the number of siblings of node n
int count_next(no* n);

//cria nos
no* create_id(char* name,no* n,int l,int c);
no* create_id_notype(char* name,int l,int c);
no* create_char(char* name,int l,int c);
no* create_reallit(char* name,int l,int c);
no* create_intlit(char* name,int l,int c);
no* create_type(char* name,int l,int c);
no* create_multi(char* name,int t,int l,int c);

void add_type_id(no_terminal* n,char* tipo);

//adiciona no inicio
void add_son_ini(no* n, no* node);

//adiciona no fim
void add_son_end(no* n, no* node);

//adiciona irmao
void add_sibling(no* n, no* node);

//print arvore
void showTree(int level, no* n);
void showTreeInfo(int level, no* n);

//liberta a memoria ocupada pela memoria
void freeMemory(no* n);

//comola o tipo defenido pelo no t em todos os nos em n
void fixVarSpec(no* n,no* t);

void printType(int t);
