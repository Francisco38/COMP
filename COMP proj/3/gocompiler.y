/*
Iago Silva Bebiano 2019219478
Francisco António da Cruz Faria 2019227649
*/
%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "functions.h"
#include "y.tab.h"

int yylex (void);
void yyerror(char* s);

extern char string[100];

extern int yyackline;
extern int yyackCol;

extern char* yytext;
struct s3* tree = NULL;

int founderror=0;
%}

%token <info> SEMICOLON COMMA BLANKID ASSIGN STAR DIV MINUS PLUS EQ GE GT LBRACE LE LPAR LSQ LT MOD NE NOT AND OR RBRACE RPAR RSQ PACKAGE RETURN ELSE FOR IF VAR INT FLOAT32 BOOL STRING PRINT PARSEINT FUNC CMDARGS
%token	<info>	IDENTIFIER RESERV INTL REAL STR
%type   <node> program Declarations VarDeclaration VarSpec VarspecRepeat Type FuncDeclaration Parameters Parameter FuncBody VarsAndStatements Statement Statement_expr StatementRepeat ParseArgs FuncInvocation FuncInvocationRepeat Expr

%right ASSIGN
%left OR
%left AND
%left XOR
%left GT GE LT LE EQ NE
%left PLUS MINUS
%left STAR DIV MOD
%right NOT

%union{
    struct s4* info;
    struct s3* node;
}
%%
program: PACKAGE IDENTIFIER SEMICOLON Declarations     {if($4!=NULL){
															tree = create_multi("Program",0,0,0);
															add_son_end(tree, $4); 
															$$ = tree;}
														else{
															tree = create_multi("Program",0,0,0); 
															$$ = tree;}
														} 
		|program PACKAGE IDENTIFIER SEMICOLON Declarations{if($5!=NULL){
															no* node =  create_multi("Program",0,0,0);
															add_son_end(node, $5); 
															add_sibling($1, node);
															$$ = $1;}
														   else{
															no* node = create_multi("Program",0,0,0);
															add_sibling($1, node); 
															$$ = $1;}
														   } 
		;

Declarations: 									{$$ = NULL;}
	| Declarations VarDeclaration SEMICOLON 	{if ($1 != NULL){
													if ($2 != NULL){
														add_sibling($1, $2);
														$$=$1;}
													else{$$=$1;}
												}else{
													if ($2 != NULL){
														$$=$2;
													}
													else{
														$$=NULL;
													}
												}} 
	| Declarations FuncDeclaration SEMICOLON	{if ($1 != NULL){
													if ($2 != NULL){
														add_sibling($1, $2); 
														$$=$1;}
													else{$$=$1;}
												}else{
													if ($2 != NULL){
														$$=$2;}
													else{$$=NULL;}
												}} 
	;

VarDeclaration:	VAR VarSpec						{$$=$2;}
	| VAR LPAR VarSpec SEMICOLON RPAR			{$$=$3;}
	;
	
VarSpec :IDENTIFIER VarspecRepeat Type			{if($2!=NULL){
													fixVarSpec($2,$3);
													no* node= create_multi("VarDecl",0,$1->linha,$1->col); 
													no* node2= create_id($1->info,$3,$1->linha,$1->col); 
													add_son_end(node, $3);
													add_son_end(node, node2);
													add_sibling(node, $2);
													$$=node;}
												else{
													no* node= create_multi("VarDecl",0,$1->linha,$1->col); 
													no* node2= create_id($1->info,$3,$1->linha,$1->col); 
													add_son_end(node, $3);
													add_son_end(node, node2);
													$$=node;}}

VarspecRepeat:VarspecRepeat COMMA IDENTIFIER	{if ($1 != NULL){
													no* node= create_multi("VarDecl",0,$3->linha,$3->col);
													no* node2= create_id($3->info,NULL,$3->linha,$3->col);
													add_son_end(node, node2); 
													add_sibling($1, node);
													$$=$1;}
											 	else{
											 		no* node= create_multi("VarDecl",0,$3->linha,$3->col);
											 		no* node2= create_id($3->info,NULL,$3->linha,$3->col);
											 		add_son_end(node, node2); 
											 		$$=node;}}
	|											{$$=NULL;}
	;

Type :INT 										{no* node= create_type("Int",$1->linha,$1->col);$$=node;}
	| FLOAT32 									{no* node= create_type("Float32",$1->linha,$1->col);$$=node;}
	| BOOL 										{no* node= create_type("Bool",$1->linha,$1->col);$$=node;}
	| STRING									{no* node= create_type("String",$1->linha,$1->col);$$=node;}
	;
	
FuncDeclaration: FUNC IDENTIFIER LPAR Parameters RPAR  FuncBody	{if($4!=NULL){
																	no* node= create_multi("FuncDecl",0,$1->linha,$1->col);
																	no* node2= create_multi("FuncHeader",0,$2->linha,$2->col);
																	no* node3= create_id($2->info,NULL,$2->linha,$2->col);
																	no* node4= create_multi("FuncParams",0,$3->linha,$3->col);
																	add_son_end(node, node2);
																	add_son_end(node2, node3);
																	add_son_end(node2, node4);
																	add_son_end(node4, $4);
																	add_sibling(node2, $6);
																	$$=node;}
																else{
																	no* node= create_multi("FuncDecl",0,$1->linha,$1->col);
																	no* node2= create_multi("FuncHeader",0,$2->linha,$2->col);
																	no* node3= create_id($2->info,NULL,$2->linha,$2->col);
																	no* node4= create_multi("FuncParams",0,$3->linha,$3->col);
																	add_son_end(node, node2);
																	add_son_end(node2, node3);
																	add_son_end(node2, node4);
																	add_sibling(node2, $6);
																	$$=node;}}
				|FUNC IDENTIFIER LPAR Parameters RPAR Type FuncBody	{if($4!=NULL){
																	no* node= create_multi("FuncDecl",0,$1->linha,$1->col);
																	no* node2= create_multi("FuncHeader",0,$2->linha,$2->col);
																	no* node3= create_id($2->info,$6,$2->linha,$2->col);
																	no* node4= create_multi("FuncParams",0,$3->linha,$3->col);
																	add_son_end(node, node2);
																	add_son_end(node2, node3);
																	add_son_end(node2, $6);
																	add_son_end(node2, node4);
																	add_son_end(node4, $4);
																	add_sibling(node2, $7);
																	$$=node;}
															 	else{
																	no* node= create_multi("FuncDecl",0,$1->linha,$1->col);
																	no* node2= create_multi("FuncHeader",0,$2->linha,$2->col);
																	no* node3= create_id($2->info,$6,$2->linha,$2->col);
																	no* node4= create_multi("FuncParams",0,$3->linha,$3->col);
																	add_son_end(node, node2);
																	add_son_end(node2, node3);
																	add_son_end(node2, $6);
																	add_son_end(node2, node4);
																	add_sibling(node2, $7);
																	$$=node;}}
		;

Parameters:			{$$=NULL;}
		|IDENTIFIER Type Parameter	{if($3!=NULL){
										no* node2= create_multi("ParamDecl",0,$1->linha,$1->col);
										no* node3= create_id($1->info,$2,$1->linha,$1->col);
										add_son_end(node2, $2);
										add_son_end(node2, node3);
										add_sibling(node2, $3);
										$$=node2;}	
									else{
										no* node2= create_multi("ParamDecl",0,$1->linha,$1->col);
										no* node3= create_id($1->info,$2,$1->linha,$1->col);
										add_son_end(node2, $2);
										add_son_end(node2, node3);
										$$=node2;}}
		;
			
Parameter:				{$$=NULL;}
		|Parameter COMMA IDENTIFIER Type	{if($1!=NULL){
												no* node= create_multi("ParamDecl",0,$3->linha,$3->col);
												no* node2= create_id($3->info,$4,$3->linha,$3->col);
												add_son_end(node, $4);
												add_son_end(node, node2);
												add_sibling($1, node);
												$$=$1;}
											else{
												no* node= create_multi("ParamDecl",0,$3->linha,$3->col);
												no* node2= create_id($3->info,$4,$3->linha,$3->col);
												add_son_end(node,$4);
												add_son_end(node, node2);
												$$=node;}}

FuncBody:	LBRACE VarsAndStatements RBRACE	{no* node= create_multi("FuncBody",0,$1->linha,$1->col);if($2!=NULL){add_son_end(node,$2);}$$=node;}
		;

VarsAndStatements:VarsAndStatements Statement SEMICOLON  	 {if($1!=NULL){
															  	if($2!=NULL){
															  		add_sibling($1, $2);
																  	$$=$1;
															  	}
																else{
																	$$=$1;
																}}
															  else{
															  	if($2!=NULL){
																  	$$=$2;
															  	}
																else{
																	$$=NULL;
																}
															  }
															 }
				|VarsAndStatements VarDeclaration SEMICOLON	 {if($1!=NULL){
																add_sibling($1, $2);
																$$=$1;}
															  else{
															  	$$=$2;
															  }}
				|VarsAndStatements SEMICOLON 				 {$$=$1;}
				| 											 {$$=NULL;}
				;
				
Statement: IDENTIFIER ASSIGN Expr												{if($3!=NULL){
																					no* node= create_multi("Assign",5,$2->linha,$2->col);
																					no* node2= create_id_notype($1->info,$1->linha,$1->col);
																					add_son_end(node, node2);
																					add_son_end(node, $3);
																					$$=node;}
																				else{
																					no* node= create_multi("Assign",0,$2->linha,$2->col);
																					no* node2= create_id_notype($1->info,$1->linha,$1->col);
																					add_son_end(node, node2);
																					$$=node;}}
		|LBRACE StatementRepeat RBRACE											{if($2!=NULL){
																					int cont=count_next($2); 
																					if(cont>1){
																						no* no=create_multi("Block",0,$1->linha,$1->col); 
																						add_son_end(no,$2);
																						$$=no;}
																					else{$$=$2;}}
																				 else{$$=NULL;}}
		|IF Expr LBRACE StatementRepeat RBRACE									{if($4!=NULL){
																						no* node= create_multi("If",0,$1->linha,$1->col);
																						add_son_end(node, $2);
																						no* node2= create_multi("Block",0,$3->linha,$3->col);
																						add_son_end(node, node2);
																						add_son_end(node2, $4);
																						no* node3= create_multi("Block",0,$5->linha,$5->col);
																						add_son_end(node, node3);
																						$$=node;}
																				else{no* node= create_multi("If",0,$1->linha,$1->col);
																						add_son_end(node, $2);
																						no* node2= create_multi("Block",0,$3->linha,$3->col);
																						add_son_end(node, node2);
																						no* node3= create_multi("Block",0,$5->linha,$5->col);
																						add_son_end(node, node3);
																						$$=node;}}
		|IF Expr LBRACE StatementRepeat RBRACE ELSE LBRACE StatementRepeat RBRACE {if($4!=NULL){
																					if($8!=NULL){
																						no* node= create_multi("If",0,$1->linha,$1->col);
																						add_son_end(node, $2);
																						no* node2= create_multi("Block",0,$3->linha,$3->col);
																						add_son_end(node, node2);
																						add_son_end(node2, $4);
																						no* node3= create_multi("Block",0,$7->linha,$7->col);
																						add_son_end(node, node3);
																						add_son_end(node3, $8);
																						$$=node;}
																					else{no* node= create_multi("If",0,$1->linha,$1->col);
																						add_son_end(node, $2);
																						no* node2= create_multi("Block",0,$3->linha,$3->col);
																						add_son_end(node, node2);
																						add_son_end(node2, $4);
																						no* node3= create_multi("Block",0,$7->linha,$7->col);
																						add_son_end(node, node3);
																						$$=node;}}
																				else{
																					if($8!=NULL){
																						no* node= create_multi("If",0,$1->linha,$1->col);
																						add_son_end(node, $2);
																						no* node2= create_multi("Block",0,$3->linha,$3->col);
																						add_son_end(node, node2);
																						add_son_end(node2, $4);
																						no* node3= create_multi("Block",0,$7->linha,$7->col);
																						add_son_end(node, node3);
																						add_son_end(node3, $8);
																						$$=node;}
																					else{
																						no* node= create_multi("If",0,$1->linha,$1->col);
																						add_son_end(node, $2);
																						no* node2= create_multi("Block",0,$3->linha,$3->col);
																						add_son_end(node, node2);
																						add_son_end(node2, $4);
																						no* node3= create_multi("Block",0,$7->linha,$7->col);
																						add_son_end(node, node3);
																						$$=node;}
																					}
																				}
		|FOR Statement_expr LBRACE StatementRepeat RBRACE						{if($2!=NULL){
																					if($4!=NULL){
																						no* node= create_multi("For",0,$1->linha,$1->col);
																						add_son_end(node, $2);
																						no* node2= create_multi("Block",0,$3->linha,$3->col);
																						add_son_end(node, node2);
																						add_son_end(node2, $4);
																						$$=node;}
																					else{no* node= create_multi("For",0,$1->linha,$1->col);
																						add_son_end(node, $2);
																						no* node2= create_multi("Block",0,$3->linha,$3->col);
																						add_son_end(node, node2);
																						$$=node;}}
																				else{
																					if($4!=NULL){
																						no* node= create_multi("For",0,$1->linha,$1->col);
																						no* node2= create_multi("Block",0,$3->linha,$3->col);
																						add_son_end(node, node2);
																						add_son_end(node2, $4);
																						$$=node;}
																					else{
																						no* node= create_multi("For",0,$1->linha,$1->col);
																						no* node2= create_multi("Block",0,$3->linha,$3->col);
																						add_son_end(node, node2);
																						$$=node;}}}
		|RETURN Statement_expr													{if($2!=NULL){
																					no* node= create_multi("Return",0,$1->linha,$1->col);
																					add_son_end(node, $2);
																					$$=node;}
																				else{
																					no* node= create_multi("Return",0,$1->linha,$1->col);
																					$$=node;}}
		|FuncInvocation 														{$$=$1;}
		|ParseArgs																{if($1!=NULL){$$=$1;}else{$$=NULL;}}
		|PRINT LPAR Expr RPAR													{no* node= create_multi("Print",0,$1->linha,$1->col);
																				 add_son_end(node, $3);
																				 $$=node;}
		|PRINT LPAR STR RPAR													{no* node= create_multi("Print",0,$1->linha,$1->col);
																				 no* node2= create_char($3->info,$3->linha,$3->col);
																				 add_son_end(node, node2);
																				 $$=node;}
		|error																	{$$ = NULL;}
		;

Statement_expr:	{$$=NULL;}
		|Expr	{$$=$1;}
		;

StatementRepeat:		{$$=NULL;}
		|StatementRepeat Statement SEMICOLON										{if($1!=NULL){
																						add_sibling($1, $2);
																						$$=$1;}
																					else{$$=$2;}}
ParseArgs: IDENTIFIER COMMA BLANKID ASSIGN PARSEINT LPAR CMDARGS LSQ Expr RSQ RPAR	{no* node= create_multi("ParseArgs",1,$5->linha,$5->col);
																					 no* node2= create_id_notype($1->info,$1->linha,$1->col);
																					 add_son_end(node, node2);
																					 add_son_end(node, $9);
																					 $$=node;}
		|IDENTIFIER COMMA BLANKID ASSIGN PARSEINT LPAR error RPAR					{$$ = NULL;}
		;	

FuncInvocation: IDENTIFIER LPAR RPAR						{no* node= create_multi("Call",5,$1->linha,$1->col);
															 no* node2= create_id_notype($1->info,$1->linha,$1->col);
															 add_son_end(node, node2);
															 $$=node;}
		|IDENTIFIER LPAR Expr FuncInvocationRepeat RPAR		{if($4!=NULL){
																if($3!=NULL){
																	no* node= create_multi("Call",5,$1->linha,$1->col);
																	no* node2= create_id_notype($1->info,$1->linha,$1->col);
																	add_son_end(node, node2);
																	add_son_end(node, $3);
																	add_son_end(node, $4);
																	$$=node;}
																else{
																	no* node= create_multi("Call",5,$1->linha,$1->col);
																	no* node2= create_id_notype($1->info,$1->linha,$1->col);
																	add_son_end(node, node2);
																	add_son_end(node, $4);
																	$$=node;}
																}
															else{if($3!=NULL){
																	no* node= create_multi("Call",5,$1->linha,$1->col);
																	no* node2= create_id_notype($1->info,$1->linha,$1->col);
																	add_son_end(node, node2);
																	add_son_end(node, $3);
																	$$=node;}
																else{
																	no* node= create_multi("Call",5,$1->linha,$1->col);
																	no* node2= create_id_notype($1->info,$1->linha,$1->col);
																	add_son_end(node, node2);
																	$$=node;}}}
		|IDENTIFIER LPAR error RPAR								{$$ = NULL;}
		;
		
FuncInvocationRepeat:FuncInvocationRepeat COMMA Expr	{if($1!=NULL){if($3!=NULL){add_sibling($1, $3);$$=$1;}else{$$=$1;}}else{if($3!=NULL){$$=$3;}else{$$=NULL;}}}
		|											{$$=NULL;}
		;

Expr :Expr OR Expr				{no* node= create_multi("Or",2,$2->linha,$2->col);
								 add_son_end(node, $1);
								 add_son_end(node, $3);
								 $$=node;}
	|Expr AND Expr				{no* node= create_multi("And",2,$2->linha,$2->col);
								 add_son_end(node, $1);
								 add_son_end(node, $3);
								 $$=node;}
	|Expr LT Expr				{no* node= create_multi("Lt",2,$2->linha,$2->col);
								 add_son_end(node, $1);
								 add_son_end(node, $3);
								 $$=node;}
	|Expr GT Expr				{no* node= create_multi("Gt",2,$2->linha,$2->col);
								 add_son_end(node, $1);
								 add_son_end(node, $3);
								 $$=node;}
	|Expr EQ Expr				{no* node= create_multi("Eq",2,$2->linha,$2->col);
								 add_son_end(node, $1);
								 add_son_end(node, $3);
								 $$=node;}
	|Expr NE Expr				{no* node= create_multi("Ne",2,$2->linha,$2->col);
								 add_son_end(node, $1);
								 add_son_end(node, $3);
								 $$=node;}
	|Expr LE Expr				{no* node= create_multi("Le",2,$2->linha,$2->col);
								 add_son_end(node, $1);
								 add_son_end(node, $3);
								 $$=node;}
	|Expr GE Expr				{no* node= create_multi("Ge",2,$2->linha,$2->col);
								 add_son_end(node, $1);
								 add_son_end(node, $3);
								 $$=node;}
	|Expr PLUS Expr				{no* node= create_multi("Add",5,$2->linha,$2->col);
								 add_son_end(node, $1);
								 add_son_end(node, $3);
								 $$=node;}
	|Expr MINUS Expr			{no* node= create_multi("Sub",5,$2->linha,$2->col);
								 add_son_end(node, $1);
								 add_son_end(node, $3);
								 $$=node;}
	|Expr STAR Expr				{no* node= create_multi("Mul",5,$2->linha,$2->col);
								 add_son_end(node, $1);
								 add_son_end(node, $3);
								 $$=node;}
	|Expr DIV Expr				{no* node= create_multi("Div",5,$2->linha,$2->col);
								 add_son_end(node, $1);
								 add_son_end(node, $3);
								 $$=node;}
	|Expr MOD Expr				{no* node= create_multi("Mod",1,$2->linha,$2->col);
								 add_son_end(node, $1);
								 add_son_end(node, $3);
								 $$=node;}
	|NOT Expr					{no* node= create_multi("Not",2,$1->linha,$1->col);
								 add_son_end(node, $2);
								 $$=node;}
	|MINUS Expr	%prec NOT		{no* node= create_multi("Minus",5,$1->linha,$1->col);
								 add_son_end(node, $2);
								 $$=node;}
	|PLUS Expr	%prec NOT		{no* node= create_multi("Plus",5,$1->linha,$1->col);
								 add_son_end(node, $2);
								 $$=node;}
	|INTL						{no* node= create_intlit($1->info,$1->linha,$1->col);$$=node;} 
	|REAL 						{no* node= create_reallit($1->info,$1->linha,$1->col);$$=node;}
	|IDENTIFIER 				{no* node= create_id_notype($1->info,$1->linha,$1->col);$$=node;}
	|FuncInvocation 			{$$=$1;}
	|LPAR Expr RPAR	    		{$$=$2;}
	|LPAR error RPAR			{$$=NULL;}
	;

%%

void yyerror(char *msg) {
	if (yychar == STR){
      printf ("Line %d, column %d: %s: \"%s\"\n",yyackline, yyackCol, msg, yylval.info->info);
    }
    else{
      printf ("Line %d, column %d: %s: %s\n", yyackline, yyackCol, msg, yytext);
    }
    founderror=1;
}

