#ifndef STRUCTURES_H
#define STRUCTURES_H

typedef struct s3* listNo;


typedef struct s1{
	char* name;
	int type;
	int linha;
	int col;
	listNo son;
	listNo next;
} no_multi;

typedef enum {id,str,intlit,reallit,type} terType;

typedef struct s2{
	char* name;
	terType tipo;
	int type;
	int linha;
	int col;
	int params[30];
	listNo next;
} no_terminal;

typedef enum {multi,terminal} tipo_no;

typedef struct s3 {
        tipo_no type;
				int error;
        union{
                no_multi* mult;
                no_terminal* ter;
        }data;
} no;

typedef struct s4{
	char* info;
	int linha;
	int col;
} infor;


#endif
