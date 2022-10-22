#include <stdio.h>
#include <stdlib.h>
#include "structures.h"

//return o porximo no
no* getNext(no* n);

//returns the number of siblings of node n
int count_next(no* n);

//cria nos
no* create_id(char* type);
no* create_char(char* type);
no* create_reallit(char* type);
no* create_intlit(char* type);
no* create_type(char* type);
no* create_multi(char* type);

//adiciona no inicio
void add_son_ini(no* n, no* node);

//adiciona no fim
void add_son_end(no* n, no* node);

//adiciona irmao
void add_sibling(no* n, no* node);

//print arvore
void showTree(int level, no* n);

//liberta a memoria ocupada pela memoria
void freeMemory(no* n);

//comola o tipo defenido pelo no t em todos os nos em n
void fixVarSpec(no* n,no* t);
