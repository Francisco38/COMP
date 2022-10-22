#ifndef STRUCTURES_H
#define STRUCTURES_H

typedef struct s3* listNo;

typedef struct s1{
	char *type;
	listNo son;
	listNo next;
} no_multi;

typedef struct s2{
	char * type;              
	listNo next;
} no_terminal;

typedef enum {multi,terminal} tipo_no;

typedef struct s3 {
        tipo_no type;
        union{                 
                no_multi* mult;
                no_terminal* ter;
        }data;
} no;

#endif
