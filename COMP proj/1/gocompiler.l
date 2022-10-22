/*
Iago Silva Bebiano 2019219478
Francisco António da Cruz Faria 2019227649
*/
%{
#include <stdio.h>
#include <ctype.h>

void erro(int type);
void show(char* s);
void show2(int type);
void cycle(int coma);

char string[100];

int print_check=0;
int autocoma=0;
int errorLine=1;
int errorCol=1;
int currentPos=1;
int foundError=0;
int inComentario=0;
int inString=0;
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
";" 				{cycle(0);show("SEMICOLON");}
"," 				{cycle(0);show("COMMA");}
"_" 				{cycle(0);show("BLANKID");}
"=" 				{cycle(0);show("ASSIGN");}
"*" 				{cycle(0);show("STAR");}
"/" 				{cycle(0);show("DIV");}
"-" 				{cycle(0);show("MINUS");}
"+" 				{cycle(0);show("PLUS");}
"==" 				{cycle(0);show("EQ");}
">=" 				{cycle(0);show("GE");}
">" 				{cycle(0);show("GT");}
"{" 				{cycle(0);show("LBRACE");}
"<=" 				{cycle(0);show("LE");}
"(" 				{cycle(0);show("LPAR");}
"[" 				{cycle(0);show("LSQ");}
"<" 				{cycle(0);show("LT");}
"%" 				{cycle(0);show("MOD");}
"!=" 				{cycle(0);show("NE");}
"!" 				{cycle(0);show("NOT");}
"&&" 				{cycle(0);show("AND");}
"||" 				{cycle(0);show("OR");}
"}" 				{cycle(1);show("RBRACE");}
")" 				{cycle(1);show("RPAR");}
"]" 				{cycle(1);show("RSQ");}
"package" 			{cycle(0);show("PACKAGE");}
"return" 			{cycle(1);show("RETURN");}
"else" 				{cycle(0);show("ELSE");}
"for" 				{cycle(0);show("FOR");}
"if" 				{cycle(0);show("IF");}
"var" 				{cycle(0);show("VAR");} 
"int" 				{cycle(0);show("INT");}
"float32" 			{cycle(0);show("FLOAT32");}
"bool" 				{cycle(0);show("BOOL");}
"string" 			{cycle(0);show("STRING");}
"fmt.Println"		{cycle(0);show("PRINT");}
"strconv.Atoi"		{cycle(0);show("PARSEINT");}
"func"				{cycle(0);show("FUNC");}
"os.Args"			{cycle(0);show("CMDARGS");}

{reserved}  		{cycle(0);show2(0);}	
{id}				{cycle(1);show2(1);}
{intlit} 			{cycle(1);show2(2);}
{reallit}			{cycle(1);show2(3);}
{invalid_octal}     {erro(1);cycle(0);}

\" 	   				{errorLine=yylineno;errorCol=currentPos;cycle(0);inString=1;BEGIN STRLIT;}
"//"   				{if(autocoma){show("SEMICOLON");autocoma=0;};cycle(0);BEGIN COMENTARIO1;}
"/*"   				{inComentario=1;errorLine=yylineno;errorCol=currentPos;cycle(0);BEGIN COMENTARIO2;} 

<STRLIT>\"          {if(!foundError){show2(4);autocoma=1;};inString=0;foundError=0;strcpy(string,"");currentPos=currentPos+yyleng;BEGIN 0;}	
<STRLIT>{escape}	{strcat(string, yytext);currentPos=currentPos+yyleng;}
<STRLIT>{newline}	{erro(3);strcpy(string,"");currentPos=1;foundError=0;inString=0;BEGIN 0;}
<STRLIT>\\.?    	{erro(4);foundError=1;currentPos=currentPos+yyleng;}
<STRLIT>.           {strcat(string, yytext);currentPos=currentPos+yyleng;}	

<COMENTARIO1>{newline}	 	{BEGIN 0;currentPos=1;}
<COMENTARIO1>. 				{;}

<COMENTARIO2>"*/" 			{currentPos=currentPos+yyleng;inComentario=0;BEGIN 0;}
<COMENTARIO2>{newline}		{if(autocoma){show("SEMICOLON");autocoma=0;};currentPos=1;}
<COMENTARIO2>. 				{currentPos=currentPos+yyleng;}

{newline}			{currentPos=1;if(autocoma){show("SEMICOLON");autocoma=0;};}
(" ")|("\t")		{currentPos=currentPos+yyleng;}
. 	 				{erro(0);cycle(0);}
%%

int main(int argc,char* argv[])
{
	if(argc==2){
		if(strcmp("-l",argv[1])==0){
			print_check=1;
		}
	}
	yylex();
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
	if(autocoma==1){
		show("SEMICOLON");
	}
	return 1;
}

void show(char* s){
	if(print_check){
		printf("%s\n",s);
	}
}

void cycle(int coma){
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
}
