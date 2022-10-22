/*
Iago Silva Bebiano 2019219478
Francisco António da Cruz Faria 2019227649
*/
%{
#include <stdio.h>
#include <ctype.h>
#include "y.tab.h"
#include "functions.h"
#include "semantics.h"
#include "codegen.h"
#include "symbol_table.h"

struct t1* program;

void erro(int type);
void show(char* s);
void show2(int type);
void cycle(int coma);
void infofill(int linha,int col);

char string[100];

int print_check=0;
int yyack_check=0;

int autocoma=0;
int errorLine=1;
int errorCol=1;
int yyackline=0;
int yyackCol=0;

int currentPos=1;
int foundError=0;
int inComentario=0;
int inString=0;
extern int founderror;
extern struct s3* tree;
%}
numero				[0-9]
letra				[a-zA-Z_]
alfanumerico		[0-9a-zA-Z_]
decimal				[1-9][0-9]*
octal				0[0-7]*
invalid_octal		0[0-9]+
hexadecimal			(("0x")|("0X"))[a-fA-F0-9]+
expoente			[eE]("+"|"-")?{numero}+

newline     		("\n")|("\r")
escape				\\(f|n|r|t|(\\)|(\"))
reserved			(("--")|("++")|("break")|("default")|("func")|("interface")|("select")|("case")|("defer")|("go")|("map")|("struct")|("chan")|("else")|("goto")|("package")|("switch")|("const")|("fallthrough")|("if")|("range")|("type")|("continue")|("for")|("import")|("return")|("var"))
id 					{letra}{alfanumerico}*
intlit              {decimal}|{hexadecimal}|{octal}
reallit				("."{numero}+{expoente}?)|({numero}+"."{numero}*{expoente}?)|({numero}+{expoente})
%option yylineno
%x COMENTARIO1 COMENTARIO2 STRLIT
%%
";" 				{cycle(0);show("SEMICOLON");if(yyack_check){infofill(yyackline,yyackCol);return SEMICOLON;};}
"," 				{cycle(0);show("COMMA");if(yyack_check){infofill(yyackline,yyackCol);return COMMA;};}
"_" 				{cycle(0);show("BLANKID");if(yyack_check){infofill(yyackline,yyackCol);return BLANKID;};}
"=" 				{cycle(0);show("ASSIGN");if(yyack_check){infofill(yyackline,yyackCol);return ASSIGN;};}
"*" 				{cycle(0);show("STAR");if(yyack_check){infofill(yyackline,yyackCol);return STAR;};}
"/" 				{cycle(0);show("DIV");if(yyack_check){infofill(yyackline,yyackCol);return DIV;};}
"-" 				{cycle(0);show("MINUS");if(yyack_check){infofill(yyackline,yyackCol);return MINUS;};}
"+" 				{cycle(0);show("PLUS");if(yyack_check){infofill(yyackline,yyackCol);return PLUS;};}
"==" 				{cycle(0);show("EQ");if(yyack_check){infofill(yyackline,yyackCol);return EQ;};}
">=" 				{cycle(0);show("GE");if(yyack_check){infofill(yyackline,yyackCol);return GE;};}
">" 				{cycle(0);show("GT");if(yyack_check){infofill(yyackline,yyackCol);return GT;};}
"{" 				{cycle(0);show("LBRACE");if(yyack_check){infofill(yyackline,yyackCol);return LBRACE;};}
"<=" 				{cycle(0);show("LE");if(yyack_check){infofill(yyackline,yyackCol);return LE;};}
"(" 				{cycle(0);show("LPAR");if(yyack_check){infofill(yyackline,yyackCol);return LPAR;};}
"[" 				{cycle(0);show("LSQ");if(yyack_check){infofill(yyackline,yyackCol);return LSQ;};}
"<" 				{cycle(0);show("LT");if(yyack_check){infofill(yyackline,yyackCol);return LT;};}
"%" 				{cycle(0);show("MOD");if(yyack_check){infofill(yyackline,yyackCol);return MOD;};}
"!=" 				{cycle(0);show("NE");if(yyack_check){infofill(yyackline,yyackCol);return NE;};}
"!" 				{cycle(0);show("NOT");if(yyack_check){infofill(yyackline,yyackCol);return NOT;};}
"&&" 				{cycle(0);show("AND");if(yyack_check){infofill(yyackline,yyackCol);return AND;};}
"||" 				{cycle(0);show("OR");if(yyack_check){infofill(yyackline,yyackCol);return OR;};}
"}" 				{cycle(1);show("RBRACE");if(yyack_check){infofill(yyackline,yyackCol);return RBRACE;};}
")" 				{cycle(1);show("RPAR");if(yyack_check){infofill(yyackline,yyackCol);return RPAR;};}
"]" 				{cycle(1);show("RSQ");if(yyack_check){infofill(yyackline,yyackCol);return RSQ;};}
"package" 			{cycle(0);show("PACKAGE");if(yyack_check){infofill(yyackline,yyackCol);return PACKAGE;};}
"return" 			{cycle(1);show("RETURN");if(yyack_check){infofill(yyackline,yyackCol);return RETURN;};}
"else" 				{cycle(0);show("ELSE");if(yyack_check){infofill(yyackline,yyackCol);return ELSE;};}
"for" 				{cycle(0);show("FOR");if(yyack_check){infofill(yyackline,yyackCol);return FOR;};}
"if" 				{cycle(0);show("IF");if(yyack_check){infofill(yyackline,yyackCol);return IF;};}
"var" 				{cycle(0);show("VAR");if(yyack_check){infofill(yyackline,yyackCol);return VAR;};}
"int" 				{cycle(0);show("INT");if(yyack_check){infofill(yyackline,yyackCol);return INT;};}
"float32" 			{cycle(0);show("FLOAT32");if(yyack_check){infofill(yyackline,yyackCol);return FLOAT32;};}
"bool" 				{cycle(0);show("BOOL");if(yyack_check){infofill(yyackline,yyackCol);return BOOL;};}
"string" 			{cycle(0);show("STRING");if(yyack_check){infofill(yyackline,yyackCol);return STRING;};}
"fmt.Println"		{cycle(0);show("PRINT");if(yyack_check){infofill(yyackline,yyackCol);return PRINT;};}
"strconv.Atoi"		{cycle(0);show("PARSEINT");if(yyack_check){infofill(yyackline,yyackCol);return PARSEINT;};}
"func"				{cycle(0);show("FUNC");if(yyack_check){infofill(yyackline,yyackCol);return FUNC;};}
"os.Args"			{cycle(0);show("CMDARGS");if(yyack_check){infofill(yyackline,yyackCol);return CMDARGS;};}

{reserved}  		{cycle(0);show2(0);if(yyack_check){return RESERV;};}
{id}				{cycle(1);show2(1);if(yyack_check){return IDENTIFIER;};}
{intlit} 			{cycle(1);show2(2);if(yyack_check){return INTL;};}
{reallit}			{cycle(1);show2(3);if(yyack_check){return REAL;};}
{invalid_octal}     {erro(1);cycle(0);if(yyack_check){return STR;};}

\" 	   				{errorLine=yylineno;errorCol=currentPos;cycle(0);inString=1;BEGIN STRLIT;}
"//"   				{BEGIN COMENTARIO1;if(autocoma){show("SEMICOLON");cycle(0);if(yyack_check){infofill(yyackline,yyackCol);return SEMICOLON;};};}
"/*"   				{inComentario=1;errorLine=yylineno;errorCol=currentPos;BEGIN COMENTARIO2;}

<STRLIT>\"          {if(!foundError){show2(4);autocoma=1;}inString=0;strcpy(string,"");currentPos=currentPos+yyleng;BEGIN 0;if(!foundError){if(yyack_check){return STR;}}foundError=0;}
<STRLIT>{escape}	{strcat(string, yytext);currentPos=currentPos+yyleng;}
<STRLIT>{newline}	{erro(3);strcpy(string,"");currentPos=1;foundError=0;inString=0;BEGIN 0;}
<STRLIT>\\.?    	{erro(4);foundError=1;currentPos=currentPos+yyleng;}
<STRLIT>.           {strcat(string, yytext);currentPos=currentPos+yyleng;}

<COMENTARIO1>{newline}	 	{BEGIN 0;currentPos=1;}
<COMENTARIO1>. 				{;}

<COMENTARIO2>"*/" 			{errorLine=yylineno;currentPos=currentPos+yyleng;inComentario=0;BEGIN 0;}
<COMENTARIO2>{newline}		{if(autocoma){show("SEMICOLON");autocoma=0;if(yyack_check){infofill(yyackline,yyackCol);return SEMICOLON;};};currentPos=1;}
<COMENTARIO2>. 				{currentPos=currentPos+yyleng;}

{newline}			{yyackCol++;errorLine=yylineno;currentPos=1;if(autocoma){show("SEMICOLON");autocoma=0;if(yyack_check){infofill(yyackline,yyackCol);return SEMICOLON;}};yyackline=yylineno;yyackCol=currentPos;}
(" ")|("\t")		{currentPos=currentPos+yyleng;}
<<EOF>>				{yyackline=yylineno;yyackCol=currentPos;if(autocoma==1){show("SEMICOLON");autocoma=0;if(yyack_check){infofill(yyackline,yyackCol);return SEMICOLON;}}return 0;}
. 	 				{erro(0);cycle(0);}
%%

int main(int argc,char* argv[])
{
	if(argc==2){
		if(strcmp("-l",argv[1])==0){
			print_check=1;
			yylex();
		}
		else if(strcmp("-t",argv[1])==0){
			yyack_check=1;
			yyparse();
			if(founderror==0){
				showTree(0,tree);
			}
			freeMemory(tree);
		}
		else if(strcmp("-s",argv[1])==0){
			yyack_check=1;
			yyparse();
			program=(struct t1*)malloc(sizeof(struct t1));
			int error_count=check_tree_top(program,tree);
			if(error_count==0){
				show_table(program);
				showTreeInfo(0,tree);
			}
			freeMemory(tree);
		}
	}
	else{
		yyack_check=1;
		yyparse();
		program=(struct t1*)malloc(sizeof(struct t1));
		int error_count=check_tree_top(program,tree);
		if(error_count==0){
			genCode(tree);
		}
		freeMemory(tree);
	}
	if(inString){
		erro(3);
	}
	if(inComentario){
		erro(2);
	}

	return 0;
}

int yywrap()
{
	return 1;
}

void show(char* s){
	if(print_check){
		printf("%s\n",s);
	}
}

void cycle(int coma){
	yyackline=yylineno;
    yyackCol=currentPos;
	autocoma=coma;
	currentPos=currentPos+yyleng;
}

void erro(int type){
	if(type==0){
		printf("Line %d, column %d: illegal character (%s)\n",yylineno,currentPos,yytext);
	}
	else if(type==1){
		printf("Line %d, column %d: invalid octal constant (%s)\n",yylineno,currentPos,yytext);
	}
	else if(type==2){
		printf("Line %d, column %d: unterminated comment\n",errorLine,errorCol);
	}
	else if(type==3){
		printf("Line %d, column %d: unterminated string literal\n",errorLine,errorCol);
	}
	else if(type==4){
		printf("Line %d, column %d: invalid escape sequence (%s)\n",yylineno,currentPos,yytext);
	}
}

void infofill(int linha,int col){
	yylval.info = (struct s4*) malloc(sizeof(struct s4));
	yylval.info->linha=linha;
	yylval.info->col=col;
}

void show2(int type){
	if(print_check){
		if(type==0){
			printf("RESERVED(%s)\n",yytext);
		}
		else if(type==1){
			printf("ID(%s)\n",yytext);
		}
		else if(type==2){
			printf("INTLIT(%s)\n",yytext);
		}
		else if(type==3){
			printf("REALLIT(%s)\n",yytext);
		}
		else if(type==4){
			printf("STRLIT(\"%s\")\n",string);
		}
	}
	if(yyack_check){
		yylval.info = (struct s4*) malloc(sizeof(struct s4));
		if(type==0){
			yylval.info->info = strdup(yytext);
			yylval.info->linha = yyackline;
			yylval.info->col =yyackCol;
		}
		else if(type==1){
			yylval.info->info = strdup(yytext);
			yylval.info->linha = yyackline;
			yylval.info->col =yyackCol;
		}
		else if(type==2){
			yylval.info->info = strdup(yytext);
			yylval.info->linha = yyackline;
			yylval.info->col =yyackCol;
		}
		else if(type==3){
			yylval.info->info = strdup(yytext);
			yylval.info->linha = yyackline;
			yylval.info->col =yyackCol;
		}
		else if(type==4){
			yylval.info->info = strdup(string);
			yylval.info->linha = yyackline;
			yylval.info->col =yyackCol;
		}
	}
}
