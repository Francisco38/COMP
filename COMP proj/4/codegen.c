#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "structures.h"
#include "codegen.h"

int varNum=1;
int ifNum=0;
int forNum=0;
int gotReturn=0;
int str_num=0;
int in_main=0;

void genCode(no* tree){
  if (tree==NULL){
    return;
  }
  genStr(tree);
  str_num=0;
  printf("@.print_int = constant [4 x i8] c\"%%d\\0A\\00\"\n");
  printf("@.print_float = constant [7 x i8] c\"%%.08f\\0A\\00\"\n");
  printf("@.print_str = constant [4 x i8] c\"%%s\\0A\\00\"\n");
  printf("@.print_bool_false = constant [7 x i8] c\"false\\0A\\00\"\n");
  printf("@.print_bool_true = constant [6 x i8] c\"true\\0A\\00\"\n");
  printf("declare i32 @printf(i8*, ...)\n");
  printf("declare i32 @atoi(...)\n");
  printf("\ndefine void @.print_bool(i1 %%bool){ \n");
  printf("  br i1 %%bool, label %%true, label %%false\n");
  printf("  true:\n");
  printf("    call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @.print_bool_true, i8 0, i8 0))\n");
  printf("    ret void\n");
  printf("  false:\n");
  printf("    call i32 (i8*, ...) @printf(i8* getelementptr ([7 x i8], [7 x i8]* @.print_bool_false, i8 0, i8 0))\n");
  printf("    ret void\n}\n\n");
  genNode(tree->data.mult->son);
}

void genStr(no* n){
  if (n == NULL) return;
  switch(n->type){
    case multi:
      genStr(n->data.mult->son);
      genStr(n->data.mult->next);
      break;
    case terminal:
      if(n->data.ter->tipo==str){
        strprint(n);
        str_num++;
      }
      genStr(n->data.ter->next);
      break;
  }
}

void strprint(no* n){
  char str_inicial[200];
  char str_final[200];
  strcpy(str_final,"");
  strcpy(str_inicial,n->data.ter->name);
  int i=0;
  for(;i<strlen(str_inicial);i++){
    if(str_inicial[i]=='\\'){
      str_final[i]=str_inicial[i];
      i++;
      switch(str_inicial[i]){
        case 'f':
          str_final[i]='0';
          i++;
          str_final[i]='C';
          break;
        case 'n':
          str_final[i]='0';
          i++;
          str_final[i]='A';
          break;
        case 'r':
          str_final[i]='0';
          i++;
          str_final[i]='D';
          break;
        case 't':
          str_final[i]='0';
          i++;
          str_final[i]='9';
          break;
        case '\\':
          str_final[i]='5';
          i++;
          str_final[i]='C';
          break;
        case '"':
          str_final[i]='2';
          i++;
          str_final[i]='2';
          break;
        default:
            break;
      }
      i++;
    }
    else{
        str_final[i]=str_inicial[i];
    }
  }
  str_final[i-1]='\0';
  printf("@.str.%d = private unnamed_addr constant [%ld x i8] c\"%s\\00\"\n",str_num,strlen(n->data.ter->name),str_final);
}

void genNode(no* node){
  if (node==NULL){
    return;
  }
  if(strcmp(node->data.mult->name,"VarDecl")==0){
		genVar(node->data.mult->son->data.ter->next);
	}
	else if(strcmp(node->data.mult->name,"FuncDecl")==0){
		genFunc(node->data.mult->son);
    varNum=1;
    ifNum=0;
    forNum=0;
    gotReturn=0;
	}
  genNode(node->data.mult->next);
}

void printTipo(int n){
  switch (n) {
    case 0:
      printf("void");
      break;
    case 1:
      printf("i32");
      break;
    case 2:
      printf("i1");
      break;
    case 3:
      printf("float");
      break;
    case 4:
      printf("i8*");
      break;
  }
}

void genVar(no* node){
  int type=node->data.ter->type;
  switch (type) {
    case 1:
      printf("@%s = global i32 0\n", node->data.ter->name);
      break;
    case 2:
      printf("@%s = global i1 false\n", node->data.ter->name);
      break;
    case 3:
      printf("@%s = global float 0.0\n", node->data.ter->name);
      break;
    case 4:
      printf("@%s = global i8* null\n", node->data.ter->name);
      break;
  }
}

void genFunc(no* node){
  genFuncDecl(node->data.mult->son);
  if(node->data.mult->next->data.mult->son!=NULL){
      printf("{\n");
      if(strcmp(node->data.mult->son->data.ter->name,"main")==0){
        in_main=1;
      }
      genFuncBodyMain(node->data.mult->next->data.mult->son);
      if(in_main==1 && gotReturn==0){
        printf("  ret i32 0\n");
      }
      in_main=0;
      printf("}\n\n");
  }
  else{
  	if(strcmp(node->data.mult->son->data.ter->name,"main")==0){
  		if(gotReturn==0){
  			printf("{\n");
  			printf("  ret i32 0\n");
  			printf("}\n\n");
  		}
    }
    else{
    	printf(" nounwind");
    }
  }
}

void genFuncDecl(no* node){
  if(strcmp(node->data.ter->name,"main")==0){
    printf("define i32 @main(i32 %%argc, i8** %%argv)");
    return;
  }
  printf("define ");
  no* params=NULL;
  int type=node->data.ter->type;
  printTipo(type);
  printf(" @%s(",node->data.ter->name);
  no* aux=node->data.ter->next;
  switch(aux->type){
		case multi:
		  params=aux;
		  break;
		case terminal:
		  params=aux->data.ter->next;
		  break;
	}
  getFuncParams(params);
  printf(")");
}

void getFuncParams(no* node){
  if(node->data.mult->son==NULL){
    return;
  }
  no* aux=node->data.mult->son;
  no* id=aux->data.mult->son->data.ter->next;
  printTipo(id->data.ter->type);
  printf(" %%%s",id->data.ter->name);
  if(aux->data.mult->next!=NULL){
    aux=aux->data.mult->next;
    while(aux->data.mult->next!=NULL){
      id=aux->data.mult->son->data.ter->next;
      printf(",");
      printTipo(id->data.ter->type);
      printf(" %%%s",id->data.ter->name);
      aux=aux->data.mult->next;
    }
    id=aux->data.mult->son->data.ter->next;
    printf(",");
    printTipo(id->data.ter->type);
    printf(" %%%s",id->data.ter->name);
  }
  return;
}

void genFuncBodyMain(no* node){
	if(node==NULL){
		return;
	}
	switch(node->type){
		case multi:
    	if(strcmp(node->data.mult->name,"VarDecl")==0){
          getVarDecl(node);
    	}
    	else if(strcmp(node->data.mult->name,"Or")==0){
          getOr(node);
    	}
      else if(strcmp(node->data.mult->name,"And")==0){
          getAnd(node);
      }
      else if(strcmp(node->data.mult->name,"Eq")==0){
          getEq(node);
      }
      else if(strcmp(node->data.mult->name,"Ne")==0){
          getNe(node);
      }
      else if(strcmp(node->data.mult->name,"Lt")==0){
          getLt(node);
      }
      else if(strcmp(node->data.mult->name,"Gt")==0){
          getGt(node);
      }
      else if(strcmp(node->data.mult->name,"Le")==0){
          getLe(node);
      }
      else if(strcmp(node->data.mult->name,"Ge")==0){
          getGe(node);
      }
    	else if(strcmp(node->data.mult->name,"Assign")==0){
          getAssign(node);
    	}
      else if(strcmp(node->data.mult->name,"Add")==0){
          getAdd(node);
    	}
      else if(strcmp(node->data.mult->name,"Sub")==0){
          getSub(node);
    	}
      else if(strcmp(node->data.mult->name,"Mul")==0){
          getMul(node);
    	}
      else if(strcmp(node->data.mult->name,"Div")==0){
          getDiv(node);
    	}
    	else if(strcmp(node->data.mult->name,"Mod")==0){
          getMod(node);
    	}
    	else if(strcmp(node->data.mult->name,"Not")==0){
          getNot(node);
    	}
      else if(strcmp(node->data.mult->name,"Minus")==0){
          getMinus(node);
      }
      else if(strcmp(node->data.mult->name,"Plus")==0){
          getPlus(node);
      }
    	else if(strcmp(node->data.mult->name,"Call")==0){
          getCall(node);
    	}
    	else if(strcmp(node->data.mult->name,"ParseArgs")==0){
          getParseArgs(node);
    	}
    	else if(strcmp(node->data.mult->name,"For")==0){
          getFor(node);
    	}
    	else if(strcmp(node->data.mult->name,"If")==0){
          getIf(node);
    	}
    	else if(strcmp(node->data.mult->name,"Return")==0){
          getReturn(node);
    	}
    	else if(strcmp(node->data.mult->name,"Print")==0){
          getPrint(node);
    	}
      genFuncBodyMain(node->data.mult->next);
    	break;
    	case terminal:

    		break;
	}
  return;
}

int genFuncBody(no* node){
	if(node==NULL){
		return 0;
	}
  int i=0;
	switch(node->type){
		case multi:
    	if(strcmp(node->data.mult->name,"VarDecl")==0){
          i=getVarDecl(node);
    	}
    	else if(strcmp(node->data.mult->name,"Or")==0){
          i=getOr(node);
    	}
      else if(strcmp(node->data.mult->name,"And")==0){
          i=getAnd(node);
      }
      else if(strcmp(node->data.mult->name,"Eq")==0){
          i=getEq(node);
      }
      else if(strcmp(node->data.mult->name,"Ne")==0){
          i=getNe(node);
      }
      else if(strcmp(node->data.mult->name,"Lt")==0){
          i=getLt(node);
      }
      else if(strcmp(node->data.mult->name,"Gt")==0){
          i=getGt(node);
      }
      else if(strcmp(node->data.mult->name,"Le")==0){
          i=getLe(node);
      }
      else if(strcmp(node->data.mult->name,"Ge")==0){
          i=getGe(node);
      }
    	else if(strcmp(node->data.mult->name,"Assign")==0){
          i=getAssign(node);
    	}
      else if(strcmp(node->data.mult->name,"Add")==0){
          i=getAdd(node);
    	}
      else if(strcmp(node->data.mult->name,"Sub")==0){
          i=getSub(node);
    	}
      else if(strcmp(node->data.mult->name,"Mul")==0){
          i=getMul(node);
    	}
      else if(strcmp(node->data.mult->name,"Div")==0){
          i=getDiv(node);
    	}
    	else if(strcmp(node->data.mult->name,"Mod")==0){
          i=getMod(node);
    	}
    	else if(strcmp(node->data.mult->name,"Not")==0){
          i=getNot(node);
    	}
      else if(strcmp(node->data.mult->name,"Minus")==0){
          i=getMinus(node);
      }
      else if(strcmp(node->data.mult->name,"Plus")==0){
          i=getPlus(node);
      }
    	else if(strcmp(node->data.mult->name,"Call")==0){
          i=getCall(node);
    	}
    	else if(strcmp(node->data.mult->name,"ParseArgs")==0){
          i=getParseArgs(node);
    	}
    	else if(strcmp(node->data.mult->name,"For")==0){
          i=getFor(node);
    	}
    	else if(strcmp(node->data.mult->name,"If")==0){
          i=getIf(node);
    	}
    	else if(strcmp(node->data.mult->name,"Return")==0){
          i=getReturn(node);
    	}
    	else if(strcmp(node->data.mult->name,"Print")==0){
          i=getPrint(node);
    	}
    	break;
    	case terminal:

    		break;
	}
  return i;
}

int getVarDecl(no* node){
    no* id=node->data.mult->son->data.ter->next;
    printf("  %%%s = alloca ",id->data.ter->name);
    printTipo(id->data.ter->type);
    printf("\n");
    id->data.ter->stored=1;
    setStored(node,id->data.ter->name);
    switch(id->data.ter->type){
      case 1:
        printf("  store i32 0, i32* %%%s\n", id->data.ter->name);
        break;
      case 2:
        printf("  store i1 false, i1* %%%s\n", id->data.ter->name);
        break;
      case 3:
        printf("  store float 0.0, float* %%%s\n", id->data.ter->name);
        break;
    }
    return 0;
}

int getOr(no* node){
  no* aux=node->data.mult->son;
  no* aux2;
  int i=0;
  int e=0;
  switch(aux->type){
    case multi:
      i=genFuncBody(aux);
      aux2=aux->data.mult->next;
      break;
    case terminal:
      aux2=aux->data.ter->next;
      if(aux->data.ter->tipo==id){
        if(aux->data.ter->stored==1){
          printf("  %%%d = load ",varNum);
          i=varNum;
          varNum++;
          printTipo(aux->data.ter->type);
          printf(", ");
          printTipo(aux->data.ter->type);
          printf("* ");
          if(aux->data.ter->isGlobal==1){
            printf("@%s\n",aux->data.ter->name);
          }
          else{
            printf("%%%s\n",aux->data.ter->name);
          }
        }
      }
      break;
  }

  switch(aux2->type){
    case multi:
      e=genFuncBody(aux2);
      break;
    case terminal:
      if(aux2->data.ter->tipo==id){
        if(aux2->data.ter->stored==1){
          printf("  %%%d = load ",varNum);
          e=varNum;
          varNum++;
          printTipo(aux2->data.ter->type);
          printf(", ");
          printTipo(aux2->data.ter->type);
          printf("* ");
          if(aux2->data.ter->isGlobal==1){
            printf("@%s\n",aux2->data.ter->name);
          }
          else{
            printf("%%%s\n",aux2->data.ter->name);
          }
        }
      }
      break;
  }

  printf("  %%%d = or ",varNum);
  varNum++;
  printTipo(node->data.mult->type);
  if(i!=0){
    printf(" %%%d,",i);
  }
  else{
    if(aux->data.ter->tipo==id){
      if(aux->data.ter->isGlobal==1){
        printf(" @%s,",aux->data.ter->name);
      }
      else{
        printf(" %%%s,",aux->data.ter->name);
      }
    }
    else{
      printf(" %s,",aux->data.ter->name);
    }
  }
  if(e!=0){
    printf(" %%%d\n",e);
  }
  else{
    if(aux2->data.ter->tipo==id){
      if(aux2->data.ter->isGlobal==1){
        printf(" @%s\n",aux2->data.ter->name);
      }
      else{
        printf(" %%%s\n",aux2->data.ter->name);
      }
    }
    else{
      printf(" %s\n",aux2->data.ter->name);
    }
  }
  return varNum-1;
}

int getAnd(no* node){
  no* aux=node->data.mult->son;
  no* aux2;
  int i=0;
  int e=0;
  switch(aux->type){
    case multi:
      i=genFuncBody(aux);
      aux2=aux->data.mult->next;
      break;
    case terminal:
      aux2=aux->data.ter->next;
      if(aux->data.ter->tipo==id){
        if(aux->data.ter->stored==1){
          printf("  %%%d = load ",varNum);
          i=varNum;
          varNum++;
          printTipo(aux->data.ter->type);
          printf(", ");
          printTipo(aux->data.ter->type);
          printf("* ");
          if(aux->data.ter->isGlobal==1){
            printf("@%s\n",aux->data.ter->name);
          }
          else{
            printf("%%%s\n",aux->data.ter->name);
          }
        }
      }
      break;
  }

  switch(aux2->type){
    case multi:
      e=genFuncBody(aux2);
      break;
    case terminal:
      if(aux2->data.ter->tipo==id){
        if(aux2->data.ter->stored==1){
          printf("  %%%d = load ",varNum);
          e=varNum;
          varNum++;
          printTipo(aux2->data.ter->type);
          printf(", ");
          printTipo(aux2->data.ter->type);
          printf("* ");
          if(aux2->data.ter->isGlobal==1){
            printf("@%s\n",aux2->data.ter->name);
          }
          else{
            printf("%%%s\n",aux2->data.ter->name);
          }
        }
      }
      break;
  }

  printf("  %%%d = and ",varNum);
  varNum++;
  printTipo(node->data.mult->type);
  if(i!=0){
    printf(" %%%d,",i);
  }
  else{
    if(aux->data.ter->tipo==id){
      if(aux->data.ter->isGlobal==1){
        printf(" @%s,",aux->data.ter->name);
      }
      else{
        printf(" %%%s,",aux->data.ter->name);
      }
    }
    else{
      printf(" %s,",aux->data.ter->name);
    }
  }
  if(e!=0){
    printf(" %%%d\n",e);
  }
  else{
    if(aux2->data.ter->tipo==id){
      if(aux2->data.ter->isGlobal==1){
        printf(" @%s\n",aux2->data.ter->name);
      }
      else{
        printf(" %%%s\n",aux2->data.ter->name);
      }
    }
    else{
      printf(" %s\n",aux2->data.ter->name);
    }
  }
  return varNum-1;
}

int getEq(no* node){
  //icmp eq <tipo> op1, op2
  no* aux=node->data.mult->son;
  no* aux2;
  int i=0;
  int e=0;
  int val=0;
  switch(aux->type){
    case multi:
      i=genFuncBody(aux);
      aux2=aux->data.mult->next;
      val=aux->data.mult->type;
      break;
    case terminal:
      val=aux->data.ter->type;
      aux2=aux->data.ter->next;
      if(aux->data.ter->tipo==id){
        if(aux->data.ter->stored==1){
          printf("  %%%d = load ",varNum);
          i=varNum;
          varNum++;
          printTipo(aux->data.ter->type);
          printf(", ");
          printTipo(aux->data.ter->type);
          printf("* ");
          if(aux->data.ter->isGlobal==1){
            printf("@%s\n",aux->data.ter->name);
          }
          else{
            printf("%%%s\n",aux->data.ter->name);
          }
        }
      }
      break;
  }

  switch(aux2->type){
    case multi:
      e=genFuncBody(aux2);
      break;
    case terminal:
      if(aux2->data.ter->tipo==id){
        if(aux2->data.ter->stored==1){
          printf("  %%%d = load ",varNum);
          e=varNum;
          varNum++;
          printTipo(aux2->data.ter->type);
          printf(", ");
          printTipo(aux2->data.ter->type);
          printf("* ");
          if(aux2->data.ter->isGlobal==1){
            printf("@%s\n",aux2->data.ter->name);
          }
          else{
            printf("%%%s\n",aux2->data.ter->name);
          }
        }
      }
      break;
  }

  printf("  %%%d = icmp eq ",varNum);
  varNum++;
  printTipo(val);
  if(i!=0){
    printf(" %%%d,",i);
  }
  else{
    if(aux->data.ter->tipo==id){
      if(aux->data.ter->isGlobal==1){
        printf(" @%s,",aux->data.ter->name);
      }
      else{
        printf(" %%%s,",aux->data.ter->name);
      }
    }
    else{
      printf(" %s,",aux->data.ter->name);
    }
  }
  if(e!=0){
    printf(" %%%d\n",e);
  }
  else{
    if(aux2->data.ter->tipo==id){
      if(aux2->data.ter->isGlobal==1){
        printf(" @%s\n",aux2->data.ter->name);
      }
      else{
        printf(" %%%s\n",aux2->data.ter->name);
      }
    }
    else{
      printf(" %s\n",aux2->data.ter->name);
    }
  }
  return varNum-1;
}

int getNe(no* node){
  //icmp ne <tipo> op1, op2
  no* aux=node->data.mult->son;
  no* aux2;
  int i=0;
  int e=0;
  int val=0;
  switch(aux->type){
    case multi:
      i=genFuncBody(aux);
      aux2=aux->data.mult->next;
      val=aux->data.mult->type;
      break;
    case terminal:
      val=aux->data.ter->type;
      aux2=aux->data.ter->next;
      if(aux->data.ter->tipo==id){
        if(aux->data.ter->stored==1){
          printf("  %%%d = load ",varNum);
          i=varNum;
          varNum++;
          printTipo(aux->data.ter->type);
          printf(", ");
          printTipo(aux->data.ter->type);
          printf("* ");
          if(aux->data.ter->isGlobal==1){
            printf("@%s\n",aux->data.ter->name);
          }
          else{
            printf("%%%s\n",aux->data.ter->name);
          }
        }
      }
      break;
  }

  switch(aux2->type){
    case multi:
      e=genFuncBody(aux2);
      break;
    case terminal:
      if(aux2->data.ter->tipo==id){
        if(aux2->data.ter->stored==1){
          printf("  %%%d = load ",varNum);
          e=varNum;
          varNum++;
          printTipo(aux2->data.ter->type);
          printf(", ");
          printTipo(aux2->data.ter->type);
          printf("* ");
          if(aux2->data.ter->isGlobal==1){
            printf("@%s\n",aux2->data.ter->name);
          }
          else{
            printf("%%%s\n",aux2->data.ter->name);
          }
        }
      }
      break;
  }

  printf("  %%%d = icmp ne ",varNum);
  varNum++;
  printTipo(val);
  if(i!=0){
    printf(" %%%d,",i);
  }
  else{
    if(aux->data.ter->tipo==id){
      if(aux->data.ter->isGlobal==1){
        printf(" @%s,",aux->data.ter->name);
      }
      else{
        printf(" %%%s,",aux->data.ter->name);
      }
    }
    else{
      printf(" %s,",aux->data.ter->name);
    }
  }
  if(e!=0){
    printf(" %%%d\n",e);
  }
  else{
    if(aux2->data.ter->tipo==id){
      if(aux2->data.ter->isGlobal==1){
        printf(" @%s\n",aux2->data.ter->name);
      }
      else{
        printf(" %%%s\n",aux2->data.ter->name);
      }
    }
    else{
      printf(" %s\n",aux2->data.ter->name);
    }
  }
  return varNum-1;
  return 0;
}

int getLt(no* node){
  //icmp slt <tipo> op1, op2
  no* aux=node->data.mult->son;
  no* aux2;
  int i=0;
  int e=0;
  int val=0;
  switch(aux->type){
    case multi:
      i=genFuncBody(aux);
      aux2=aux->data.mult->next;
      val=aux->data.mult->type;
      break;
    case terminal:
      val=aux->data.ter->type;
      aux2=aux->data.ter->next;
      if(aux->data.ter->tipo==id){
        if(aux->data.ter->stored==1){
          printf("  %%%d = load ",varNum);
          i=varNum;
          varNum++;
          printTipo(aux->data.ter->type);
          printf(", ");
          printTipo(aux->data.ter->type);
          printf("* ");
          if(aux->data.ter->isGlobal==1){
            printf("@%s\n",aux->data.ter->name);
          }
          else{
            printf("%%%s\n",aux->data.ter->name);
          }
        }
      }
      break;
  }

  switch(aux2->type){
    case multi:
      e=genFuncBody(aux2);
      break;
    case terminal:
      if(aux2->data.ter->tipo==id){
        if(aux2->data.ter->stored==1){
          printf("  %%%d = load ",varNum);
          e=varNum;
          varNum++;
          printTipo(aux2->data.ter->type);
          printf(", ");
          printTipo(aux2->data.ter->type);
          printf("* ");
          if(aux2->data.ter->isGlobal==1){
            printf("@%s\n",aux2->data.ter->name);
          }
          else{
            printf("%%%s\n",aux2->data.ter->name);
          }
        }
      }
      break;
  }
  printf("  %%%d = icmp slt ",varNum);
  varNum++;
  printTipo(val);
  if(i!=0){
    printf(" %%%d,",i);
  }
  else{
    if(aux->data.ter->tipo==id){
      if(aux->data.ter->isGlobal==1){
        printf(" @%s,",aux->data.ter->name);
      }
      else{
        printf(" %%%s,",aux->data.ter->name);
      }
    }
    else{
      printf(" %s,",aux->data.ter->name);
    }
  }
  if(e!=0){
    printf(" %%%d\n",e);
  }
  else{
    if(aux2->data.ter->tipo==id){
      if(aux2->data.ter->isGlobal==1){
        printf(" @%s\n",aux2->data.ter->name);
      }
      else{
        printf(" %%%s\n",aux2->data.ter->name);
      }
    }
    else{
      printf(" %s\n",aux2->data.ter->name);
    }
  }
  return varNum-1;
  return 0;
}

int getGt(no* node){
  //icmp sgt <tipo> op1, op2
  no* aux=node->data.mult->son;
  no* aux2;
  int i=0;
  int e=0;
  int val=0;
  switch(aux->type){
    case multi:
      val=aux->data.mult->type;
      i=genFuncBody(aux);
      aux2=aux->data.mult->next;
      break;
    case terminal:
      val=aux->data.ter->type;
      aux2=aux->data.ter->next;
      if(aux->data.ter->tipo==id){
        if(aux->data.ter->stored==1){
          printf("  %%%d = load ",varNum);
          i=varNum;
          varNum++;
          printTipo(aux->data.ter->type);
          printf(", ");
          printTipo(aux->data.ter->type);
          printf("* ");
          if(aux->data.ter->isGlobal==1){
            printf("@%s\n",aux->data.ter->name);
          }
          else{
            printf("%%%s\n",aux->data.ter->name);
          }
        }
      }
      break;
  }

  switch(aux2->type){
    case multi:
      e=genFuncBody(aux2);
      break;
    case terminal:
      if(aux2->data.ter->tipo==id){
        if(aux2->data.ter->stored==1){
          printf("  %%%d = load ",varNum);
          e=varNum;
          varNum++;
          printTipo(aux2->data.ter->type);
          printf(", ");
          printTipo(aux2->data.ter->type);
          printf("* ");
          if(aux2->data.ter->isGlobal==1){
            printf("@%s\n",aux2->data.ter->name);
          }
          else{
            printf("%%%s\n",aux2->data.ter->name);
          }
        }
      }
      break;
  }

  printf("  %%%d = icmp sgt ",varNum);
  varNum++;
  printTipo(val);
  if(i!=0){
    printf(" %%%d,",i);
  }
  else{
    if(aux->data.ter->tipo==id){
      if(aux->data.ter->isGlobal==1){
        printf(" @%s,",aux->data.ter->name);
      }
      else{
        printf(" %%%s,",aux->data.ter->name);
      }
    }
    else{
      printf(" %s,",aux->data.ter->name);
    }
  }
  if(e!=0){
    printf(" %%%d\n",e);
  }
  else{
    if(aux2->data.ter->tipo==id){
      if(aux2->data.ter->isGlobal==1){
        printf(" @%s\n",aux2->data.ter->name);
      }
      else{
        printf(" %%%s\n",aux2->data.ter->name);
      }
    }
    else{
      printf(" %s\n",aux2->data.ter->name);
    }
  }
  return varNum-1;
  return 0;
}

int getLe(no* node){
  //icmp sle <tipo> op1, op2
  no* aux=node->data.mult->son;
  no* aux2;
  int i=0;
  int e=0;
  int val=0;
  switch(aux->type){
    case multi:
      val=aux->data.mult->type;
      i=genFuncBody(aux);
      aux2=aux->data.mult->next;
      break;
    case terminal:
      val=aux->data.ter->type;
      aux2=aux->data.ter->next;
      if(aux->data.ter->tipo==id){
        if(aux->data.ter->stored==1){
          printf("  %%%d = load ",varNum);
          i=varNum;
          varNum++;
          printTipo(aux->data.ter->type);
          printf(", ");
          printTipo(aux->data.ter->type);
          printf("* ");
          if(aux->data.ter->isGlobal==1){
            printf("@%s\n",aux->data.ter->name);
          }
          else{
            printf("%%%s\n",aux->data.ter->name);
          }
        }
      }
      break;
  }

  switch(aux2->type){
    case multi:
      e=genFuncBody(aux2);
      break;
    case terminal:
      if(aux2->data.ter->tipo==id){
        if(aux2->data.ter->stored==1){
          printf("  %%%d = load ",varNum);
          e=varNum;
          varNum++;
          printTipo(aux2->data.ter->type);
          printf(", ");
          printTipo(aux2->data.ter->type);
          printf("* ");
          if(aux2->data.ter->isGlobal==1){
            printf("@%s\n",aux2->data.ter->name);
          }
          else{
            printf("%%%s\n",aux2->data.ter->name);
          }
        }
      }
      break;
  }

  printf("  %%%d = icmp sle ",varNum);
  varNum++;
  printTipo(val);
  if(i!=0){
    printf(" %%%d,",i);
  }
  else{
    if(aux->data.ter->tipo==id){
      if(aux->data.ter->isGlobal==1){
        printf(" @%s,",aux->data.ter->name);
      }
      else{
        printf(" %%%s,",aux->data.ter->name);
      }
    }
    else{
      printf(" %s,",aux->data.ter->name);
    }
  }
  if(e!=0){
    printf(" %%%d\n",e);
  }
  else{
    if(aux2->data.ter->tipo==id){
      if(aux2->data.ter->isGlobal==1){
        printf(" @%s\n",aux2->data.ter->name);
      }
      else{
        printf(" %%%s\n",aux2->data.ter->name);
      }
    }
    else{
      printf(" %s\n",aux2->data.ter->name);
    }
  }
  return varNum-1;
  return 0;
}

int getGe(no* node){
  //icmp sge <tipo> op1, op2
  no* aux=node->data.mult->son;
  no* aux2;
  int i=0;
  int e=0;
  int val=0;
  switch(aux->type){
      case multi:
        val=aux->data.mult->type;
        i=genFuncBody(aux);
        aux2=aux->data.mult->next;
        break;
      case terminal:
        val=aux->data.ter->type;
        aux2=aux->data.ter->next;
        if(aux->data.ter->tipo==id){
          if(aux->data.ter->stored==1){
            printf("  %%%d = load ",varNum);
            i=varNum;
            varNum++;
            printTipo(aux->data.ter->type);
            printf(", ");
            printTipo(aux->data.ter->type);
            printf("* ");
            if(aux->data.ter->isGlobal==1){
              printf("@%s\n",aux->data.ter->name);
            }
            else{
              printf("%%%s\n",aux->data.ter->name);
            }
          }
        }
        break;
    }

    switch(aux2->type){
      case multi:
        e=genFuncBody(aux2);
        break;
      case terminal:
        if(aux2->data.ter->tipo==id){
          if(aux2->data.ter->stored==1){
            printf("  %%%d = load ",varNum);
            e=varNum;
            varNum++;
            printTipo(aux2->data.ter->type);
            printf(", ");
            printTipo(aux2->data.ter->type);
            printf("* ");
            if(aux2->data.ter->isGlobal==1){
              printf("@%s\n",aux2->data.ter->name);
            }
            else{
              printf("%%%s\n",aux2->data.ter->name);
            }
          }
        }
        break;
    }

  printf("  %%%d = icmp sge ",varNum);
  varNum++;
  printTipo(val);
  if(i!=0){
    printf(" %%%d,",i);
  }
  else{
    if(aux->data.ter->tipo==id){
      if(aux->data.ter->isGlobal==1){
        printf(" @%s,",aux->data.ter->name);
      }
      else{
        printf(" %%%s,",aux->data.ter->name);
      }
    }
    else{
      printf(" %s,",aux->data.ter->name);
    }
  }
  if(e!=0){
    printf(" %%%d\n",e);
  }
  else{
    if(aux2->data.ter->tipo==id){
      if(aux2->data.ter->isGlobal==1){
        printf(" @%s\n",aux2->data.ter->name);
      }
      else{
        printf(" %%%s\n",aux2->data.ter->name);
      }
    }
    else{
      printf(" %s\n",aux2->data.ter->name);
    }
  }
  return varNum-1;
  return 0;
}

int getAdd(no* node){
  //%2 = add i32 %1, 2
  no* aux=node->data.mult->son;
  no* aux2;
  int i=0;
  int e=0;
  switch(aux->type){
    case multi:
      i=genFuncBody(aux);
      aux2=aux->data.mult->next;
      break;
    case terminal:
      aux2=aux->data.ter->next;
      if(aux->data.ter->tipo==id){
        if(aux->data.ter->stored==1){
          printf("  %%%d = load ",varNum);
          i=varNum;
          varNum++;
          printTipo(aux->data.ter->type);
          printf(", ");
          printTipo(aux->data.ter->type);
          printf("* ");
          if(aux->data.ter->isGlobal==1){
            printf("@%s\n",aux->data.ter->name);
          }
          else{
            printf("%%%s\n",aux->data.ter->name);
          }
        }
      }
      if(aux->data.ter->tipo==reallit){
          printf("  %%%d = fptrunc double %s to float\n",varNum,aux->data.ter->name);
          i=varNum;
          varNum++;
      }
      break;
  }

  switch(aux2->type){
    case multi:
      e=genFuncBody(aux2);
      break;
    case terminal:
      if(aux2->data.ter->tipo==id){
        if(aux2->data.ter->stored==1){
          printf("  %%%d = load ",varNum);
          e=varNum;
          varNum++;
          printTipo(aux2->data.ter->type);
          printf(", ");
          printTipo(aux2->data.ter->type);
          printf("* ");
          if(aux2->data.ter->isGlobal==1){
            printf("@%s\n",aux2->data.ter->name);
          }
          else{
            printf("%%%s\n",aux2->data.ter->name);
          }
        }
      }
      if(aux2->data.ter->tipo==reallit){
          printf("  %%%d = fptrunc double %s to float\n",varNum,aux2->data.ter->name);
          e=varNum;
          varNum++;
      }
      break;
  }

  if(node->data.mult->type==3){
    printf("  %%%d = fadd float",varNum);
  }
  else{
    printf("  %%%d = add ",varNum);
    printTipo(node->data.mult->type);
  }
  varNum++;
  if(i!=0){
    printf(" %%%d,",i);
  }
  else{
    if(aux->data.ter->tipo==id){
      if(aux->data.ter->isGlobal==1){
        printf(" @%s,",aux->data.ter->name);
      }
      else{
        printf(" %%%s,",aux->data.ter->name);
      }
    }
    else{
      printf(" %s,",aux->data.ter->name);
    }
  }
  if(e!=0){
    printf(" %%%d\n",e);
  }
  else{
    if(aux2->data.ter->tipo==id){
      if(aux2->data.ter->isGlobal==1){
        printf(" @%s\n",aux2->data.ter->name);
      }
      else{
        printf(" %%%s\n",aux2->data.ter->name);
      }
    }
    else{
      printf(" %s\n",aux2->data.ter->name);
    }
  }
  return varNum-1;
}

int getSub(no* node){
  //%2 = sub i32 %1, 2
  no* aux=node->data.mult->son;
  no* aux2;
  int i=0;
  int e=0;
  switch(aux->type){
    case multi:
      i=genFuncBody(aux);
      aux2=aux->data.mult->next;
      break;
    case terminal:
      aux2=aux->data.ter->next;
      if(aux->data.ter->tipo==id){
        if(aux->data.ter->stored==1){
          printf("  %%%d = load ",varNum);
          i=varNum;
          varNum++;
          printTipo(aux->data.ter->type);
          printf(", ");
          printTipo(aux->data.ter->type);
          printf("* ");
          if(aux->data.ter->isGlobal==1){
            printf("@%s\n",aux->data.ter->name);
          }
          else{
            printf("%%%s\n",aux->data.ter->name);
          }
        }
      }
      if(aux->data.ter->tipo==reallit){
          printf("  %%%d = fptrunc double %s to float\n",varNum,aux->data.ter->name);
          i=varNum;
          varNum++;
      }
      break;
  }

  switch(aux2->type){
    case multi:
      e=genFuncBody(aux2);
      break;
    case terminal:
      if(aux2->data.ter->tipo==id){
        if(aux2->data.ter->stored==1){
          printf("  %%%d = load ",varNum);
          e=varNum;
          varNum++;
          printTipo(aux2->data.ter->type);
          printf(", ");
          printTipo(aux2->data.ter->type);
          printf("* ");
          if(aux2->data.ter->isGlobal==1){
            printf("@%s\n",aux2->data.ter->name);
          }
          else{
            printf("%%%s\n",aux2->data.ter->name);
          }
        }
      }
      if(aux2->data.ter->tipo==reallit){
          printf("  %%%d = fptrunc double %s to float\n",varNum,aux2->data.ter->name);
          e=varNum;
          varNum++;
      }
      break;
  }

  if(node->data.mult->type==3){
    printf("  %%%d = fsub float",varNum);
  }
  else{
    printf("  %%%d = sub ",varNum);
    printTipo(node->data.mult->type);
  }
  varNum++;
  if(i!=0){
    printf(" %%%d,",i);
  }
  else{
    if(aux->data.ter->tipo==id){
      if(aux->data.ter->isGlobal==1){
        printf(" @%s,",aux->data.ter->name);
      }
      else{
        printf(" %%%s,",aux->data.ter->name);
      }
    }
    else{
      printf(" %s,",aux->data.ter->name);
    }
  }
  if(e!=0){
    printf(" %%%d\n",e);
  }
  else{
    if(aux2->data.ter->tipo==id){
      if(aux2->data.ter->isGlobal==1){
        printf(" @%s\n",aux2->data.ter->name);
      }
      else{
        printf(" %%%s\n",aux2->data.ter->name);
      }
    }
    else{
      printf(" %s\n",aux2->data.ter->name);
    }
  }
  return varNum-1;
}

int getMul(no* node){
  //%1 = mul i32 2, 8
  no* aux=node->data.mult->son;
  no* aux2;
  int i=0;
  int e=0;
  switch(aux->type){
    case multi:
      i=genFuncBody(aux);
      aux2=aux->data.mult->next;
      break;
    case terminal:
      aux2=aux->data.ter->next;
      if(aux->data.ter->tipo==id){
        if(aux->data.ter->stored==1){
          printf("  %%%d = load ",varNum);
          i=varNum;
          varNum++;
          printTipo(aux->data.ter->type);
          printf(", ");
          printTipo(aux->data.ter->type);
          printf("* ");
          if(aux->data.ter->isGlobal==1){
            printf("@%s\n",aux->data.ter->name);
          }
          else{
            printf("%%%s\n",aux->data.ter->name);
          }
        }
      }
      if(aux->data.ter->tipo==reallit){
          printf("  %%%d = fptrunc double %s to float\n",varNum,aux->data.ter->name);
          i=varNum;
          varNum++;
      }
      break;
  }

  switch(aux2->type){
    case multi:
      e=genFuncBody(aux2);
      break;
    case terminal:
      if(aux2->data.ter->tipo==id){
        if(aux2->data.ter->stored==1){
          printf("  %%%d = load ",varNum);
          e=varNum;
          varNum++;
          printTipo(aux2->data.ter->type);
          printf(", ");
          printTipo(aux2->data.ter->type);
          printf("* ");
          if(aux2->data.ter->isGlobal==1){
            printf("@%s\n",aux2->data.ter->name);
          }
          else{
            printf("%%%s\n",aux2->data.ter->name);
          }
        }
      }
      if(aux2->data.ter->tipo==reallit){
          printf("  %%%d = fptrunc double %s to float\n",varNum,aux2->data.ter->name);
          e=varNum;
          varNum++;
      }
      break;
  }

  if(node->data.mult->type==3){
    printf("  %%%d = fmul float",varNum);
  }
  else{
    printf("  %%%d = mul ",varNum);
    printTipo(node->data.mult->type);
  }
  varNum++;
  if(i!=0){
    printf(" %%%d,",i);
  }
  else{
    if(aux->data.ter->tipo==id){
      if(aux->data.ter->isGlobal==1){
        printf(" @%s,",aux->data.ter->name);
      }
      else{
        printf(" %%%s,",aux->data.ter->name);
      }
    }
    else{
      printf(" %s,",aux->data.ter->name);
    }
  }
  if(e!=0){
    printf(" %%%d\n",e);
  }
  else{
    if(aux2->data.ter->tipo==id){
      if(aux2->data.ter->isGlobal==1){
        printf(" @%s\n",aux2->data.ter->name);
      }
      else{
        printf(" %%%s\n",aux2->data.ter->name);
      }
    }
    else{
      printf(" %s\n",aux2->data.ter->name);
    }
  }
  return varNum-1;
}

int getDiv(no* node){
  //%2 = udiv i32 %1, 2
  no* aux=node->data.mult->son;
  no* aux2;
  int i=0;
  int e=0;
  switch(aux->type){
    case multi:
      i=genFuncBody(aux);
      aux2=aux->data.mult->next;
      break;
    case terminal:
      aux2=aux->data.ter->next;
      if(aux->data.ter->tipo==id){
        if(aux->data.ter->stored==1){
          printf("  %%%d = load ",varNum);
          i=varNum;
          varNum++;
          printTipo(aux->data.ter->type);
          printf(", ");
          printTipo(aux->data.ter->type);
          printf("* ");
          if(aux->data.ter->isGlobal==1){
            printf("@%s\n",aux->data.ter->name);
          }
          else{
            printf("%%%s\n",aux->data.ter->name);
          }
        }
      }
      if(aux->data.ter->tipo==reallit){
          printf("  %%%d = fptrunc double %s to float\n",varNum,aux->data.ter->name);
          i=varNum;
          varNum++;
      }
      break;
  }

  switch(aux2->type){
    case multi:
      e=genFuncBody(aux2);
      break;
    case terminal:
      if(aux2->data.ter->tipo==id){
        if(aux2->data.ter->stored==1){
          printf("  %%%d = load ",varNum);
          e=varNum;
          varNum++;
          printTipo(aux2->data.ter->type);
          printf(", ");
          printTipo(aux2->data.ter->type);
          printf("* ");
          if(aux2->data.ter->isGlobal==1){
            printf("@%s\n",aux2->data.ter->name);
          }
          else{
            printf("%%%s\n",aux2->data.ter->name);
          }
        }
      }
      if(aux2->data.ter->tipo==reallit){
          printf("  %%%d = fptrunc double %s to float\n",varNum,aux2->data.ter->name);
          e=varNum;
          varNum++;
      }
      break;
  }

  if(node->data.mult->type==3){
    printf("  %%%d = fdiv float",varNum);
  }
  else{
    printf("  %%%d = udiv ",varNum);
    printTipo(node->data.mult->type);
  }
  varNum++;
  if(i!=0){
    printf(" %%%d,",i);
  }
  else{
    if(aux->data.ter->tipo==id){
      if(aux->data.ter->isGlobal==1){
        printf(" @%s,",aux->data.ter->name);
      }
      else{
        printf(" %%%s,",aux->data.ter->name);
      }
    }
    else{
      printf(" %s,",aux->data.ter->name);
    }
  }
  if(e!=0){
    printf(" %%%d\n",e);
  }
  else{
    if(aux2->data.ter->tipo==id){
      if(aux2->data.ter->isGlobal==1){
        printf(" @%s\n",aux2->data.ter->name);
      }
      else{
        printf(" %%%s\n",aux2->data.ter->name);
      }
    }
    else{
      printf(" %s\n",aux2->data.ter->name);
    }
  }
  return varNum-1;
}

int getMod(no* node){
  no* aux=node->data.mult->son;
  no* aux2;
  int i=0;
  int e=0;
  switch(aux->type){
    case multi:
      i=genFuncBody(aux);
      aux2=aux->data.mult->next;
      break;
    case terminal:
      aux2=aux->data.ter->next;
      if(aux->data.ter->tipo==id){
        if(aux->data.ter->stored==1){
          printf("  %%%d = load ",varNum);
          i=varNum;
          varNum++;
          printTipo(aux->data.ter->type);
          printf(", ");
          printTipo(aux->data.ter->type);
          printf("* ");
          if(aux->data.ter->isGlobal==1){
            printf("@%s\n",aux->data.ter->name);
          }
          else{
            printf("%%%s\n",aux->data.ter->name);
          }
        }
      }
      if(aux->data.ter->tipo==reallit){
          printf("  %%%d = fptrunc double %s to float\n",varNum,aux->data.ter->name);
          i=varNum;
          varNum++;
      }
      break;
  }

  switch(aux2->type){
    case multi:
      e=genFuncBody(aux2);
      break;
    case terminal:
      if(aux2->data.ter->tipo==id){
        if(aux2->data.ter->stored==1){
          printf("  %%%d = load ",varNum);
          e=varNum;
          varNum++;
          printTipo(aux2->data.ter->type);
          printf(", ");
          printTipo(aux2->data.ter->type);
          printf("* ");
          if(aux2->data.ter->isGlobal==1){
            printf("@%s\n",aux2->data.ter->name);
          }
          else{
            printf("%%%s\n",aux2->data.ter->name);
          }
        }
      }
      if(aux2->data.ter->tipo==reallit){
          printf("  %%%d = fptrunc double %s to float\n",varNum,aux2->data.ter->name);
          e=varNum;
          varNum++;
      }
      break;
  }

  if(node->data.mult->type==3){
    printf("  %%%d = frem float",varNum);
  }
  else{
    printf("  %%%d = urem ",varNum);
    printTipo(node->data.mult->type);
  }
  varNum++;
  if(i!=0){
    printf(" %%%d,",i);
  }
  else{
    if(aux->data.ter->tipo==id){
      if(aux->data.ter->isGlobal==1){
        printf(" @%s,",aux->data.ter->name);
      }
      else{
        printf(" %%%s,",aux->data.ter->name);
      }
    }
    else{
      printf(" %s,",aux->data.ter->name);
    }
  }
  if(e!=0){
    printf(" %%%d\n",e);
  }
  else{
    if(aux2->data.ter->tipo==id){
      if(aux2->data.ter->isGlobal==1){
        printf(" @%s\n",aux2->data.ter->name);
      }
      else{
        printf(" %%%s\n",aux2->data.ter->name);
      }
    }
    else{
      printf(" %s\n",aux2->data.ter->name);
    }
  }
  return varNum-1;
}

int getNot(no* node){
  no* aux=node->data.mult->son;
  int i=0;
  int val=0;
  switch(aux->type){
    case multi:
      i=genFuncBody(aux);
      val=aux->data.mult->type;
      break;
    case terminal:
      val=aux->data.ter->type;
      if(aux->data.ter->tipo==id){
        if(aux->data.ter->stored==1){
          printf("  %%%d = load ",varNum);
          i=varNum;
          varNum++;
          printTipo(aux->data.ter->type);
          printf(", ");
          printTipo(aux->data.ter->type);
          printf("* ");
          if(aux->data.ter->isGlobal==1){
            printf("@%s\n",aux->data.ter->name);
          }
          else{
            printf("%%%s\n",aux->data.ter->name);
          }
        }
      }
      break;
  }
  printf("  %%%d = icmp eq i1 1,1\n",varNum);
  varNum++;
  printf("  %%%d = icmp ne ",varNum);
  varNum++;
  printTipo(val);
  if(i!=0){
    printf(" %%%d,",i);
  }
  else{
    if(aux->data.ter->tipo==id){
      if(aux->data.ter->isGlobal==1){
        printf(" @%s,",aux->data.ter->name);
      }
      else{
        printf(" %%%s,",aux->data.ter->name);
      }
    }
    else{
      printf(" %s,",aux->data.ter->name);
    }
  }
  printf("%%%d\n",varNum-2);
  return varNum-1;
}

int getMinus(no* node){
  no* aux=node->data.mult->son;
  int i=0;
  switch(aux->type){
    case multi:
      i=genFuncBody(aux);
      break;
    case terminal:
      if(aux->data.ter->tipo==id){
        if(aux->data.ter->stored==1){
          printf("  %%%d = load ",varNum);
          i=varNum;
          varNum++;
          printTipo(aux->data.ter->type);
          printf(", ");
          printTipo(aux->data.ter->type);
          printf("* ");
          if(aux->data.ter->isGlobal==1){
            printf("@%s\n",aux->data.ter->name);
          }
          else{
            printf("%%%s\n",aux->data.ter->name);
          }
        }
      }
      break;
  }

  printf("  %%%d = mul ",varNum);
  varNum++;
  printTipo(node->data.mult->type);
  if(i!=0){
    printf(" %%%d,",i);
  }
  else{
    if(aux->data.ter->tipo==id){
      if(aux->data.ter->isGlobal==1){
        printf(" @%s,",aux->data.ter->name);
      }
      else{
        printf(" %%%s,",aux->data.ter->name);
      }
    }
    else{
      printf(" %s,",aux->data.ter->name);
    }
  }
  printf("-1\n");
  return varNum-1;
}

int getPlus(no* node){
  no* aux=node->data.mult->son;
  int i=0;
  switch(aux->type){
    case multi:
      i=genFuncBody(aux);
      break;
    case terminal:
      if(aux->data.ter->tipo==id){
        if(aux->data.ter->stored==1){
          printf("  %%%d = load ",varNum);
          i=varNum;
          varNum++;
          printTipo(aux->data.ter->type);
          printf(", ");
          printTipo(aux->data.ter->type);
          printf("* ");
          if(aux->data.ter->isGlobal==1){
            printf("@%s\n",aux->data.ter->name);
          }
          else{
            printf("%%%s\n",aux->data.ter->name);
          }
        }
      }
      break;
  }

  printf("  %%%d = mul ",varNum);
  varNum++;
  printTipo(node->data.mult->type);
  if(i!=0){
    printf(" %%%d,",i);
  }
  else{
    if(aux->data.ter->tipo==id){
      if(aux->data.ter->isGlobal==1){
        printf(" @%s,",aux->data.ter->name);
      }
      else{
        printf(" %%%s,",aux->data.ter->name);
      }
    }
    else{
      printf(" %s,",aux->data.ter->name);
    }
  }
  printf("1\n");
  return varNum-1;
}

int getCall(no* node){
  //%1 = call i32 @sum(i32 1, i32 2, i32 3)
  no* id_node=node->data.mult->son;
  char info[2000];
  char str[50];
  strcpy(info,"call ");
  int i=0;
  switch (node->data.mult->type) {
    case 0:
      strcat(info,"void @");
      break;
    case 1:
      strcat(info,"i32 @");
      break;
    case 2:
      strcat(info,"i1 @");
      break;
    case 3:
      strcat(info,"float @");
      break;
    case 4:
      strcat(info,"i8* @");
      break;
  }
  strcat(info,id_node->data.ter->name);
  strcat(info,"(");
  no* aux=id_node->data.ter->next;
  while(aux!=NULL){
    switch(aux->type){
      case multi:
        switch (aux->data.mult->type) {
          case 0:
            strcat(info,"void ");
            break;
          case 1:
            strcat(info,"i32 ");
            break;
          case 2:
            strcat(info,"i1 ");
            break;
          case 3:
            strcat(info,"float ");
            break;
          case 4:
            strcat(info,"i8* ");
            break;
        }
        i=genFuncBody(aux);
        sprintf (str, " %%%d", i);
        strcat(info,str);
        if(aux->data.mult->next!=NULL){
            strcat(info,", ");
        }
        aux=aux->data.mult->next;
        break;
      case terminal:
        switch (aux->data.ter->type) {
          case 0:
            strcat(info,"void ");
            break;
          case 1:
            strcat(info,"i32 ");
            break;
          case 2:
            strcat(info,"i1 ");
            break;
          case 3:
            strcat(info,"float ");
            break;
          case 4:
            strcat(info,"i8* ");
            break;
        }
        if(aux->data.ter->tipo==id){
          if(aux->data.ter->stored==1){
            printf("  %%%d = load ",varNum);
            varNum++;
            printTipo(aux->data.ter->type);
            printf(", ");
            printTipo(aux->data.ter->type);
            printf("* ");
            if(aux->data.ter->isGlobal==1){
              printf("@%s\n",aux->data.ter->name);
            }
            else{
              printf("%%%s\n",aux->data.ter->name);
            }
            sprintf (str, " %%%d", varNum-1);
            strcat(info,str);
          }
          else{
            if(aux->data.ter->isGlobal==1){
              strcat(info,"@");
            }
            else{
              strcat(info,"%");
            }
            strcat(info,aux->data.ter->name);
          }
        }
        else{
          strcat(info,aux->data.ter->name);
        }
        if(aux->data.ter->next!=NULL){
            strcat(info,", ");
        }
        aux=aux->data.ter->next;
        break;
    }
  }
  strcat(info,")\n");
  printf("  %%%d = %s",varNum,info);
  varNum++;
  return varNum-1;
}

int getParseArgs(no* node){
  no* aux=node->data.mult->son;
  no* aux2=aux->data.ter->next;
  printf("  %%%d = getelementptr i8*, i8** %%argv, i64 %s\n",varNum,aux2->data.ter->name);
  printf("  %%%d = load i8*, i8** %%%d\n",varNum+1,varNum);
  printf("  %%%d = call i32 (i8*, ...) bitcast (i32 (...)* @atoi to i32 (i8*, ...)*)(i8* %%%d)\n",varNum+2,varNum+1);
  varNum=varNum+2;
  printf("  store ");
  printTipo(aux->data.ter->type);
  printf(" %%%d,",varNum);
  printTipo(aux->data.ter->type);
  printf("* ");
  if(aux->data.ter->isGlobal==1){
    printf("@%s\n",aux->data.ter->name);
  }
  else{
    printf("%%%s\n",aux->data.ter->name);
  }
  varNum++;
  return 0;
}

int getFor(no* node){
  no* aux=node->data.mult->son;
  int i=0;
  printf("  br label %%for%d\n",forNum);
  printf("for%d:\n",forNum);
  switch(aux->type){
    case multi:
      i=genFuncBody(aux);
      if(strcmp(aux->data.mult->name,"Block")!=0){
          printf("  br i1 %%%d, label %%forfunc%d, label %%contfor%d\n",i,forNum,forNum);
          aux=aux->data.mult->next;
      }
      else{
          printf("  br label %%forfunc%d\n",forNum);
      }
      break;
    case terminal:
      if(aux->data.ter->tipo==id){
        printf("  %%%d = load ",varNum);
        varNum++;
        printTipo(aux->data.ter->type);
        printf(", ");
        printTipo(aux->data.ter->type);
        printf("* ");
        if(aux->data.ter->isGlobal==1){
          printf(" @%s\n",aux->data.ter->name);
        }
        else{
          printf(" %%%s\n",aux->data.ter->name);
        }
        printf("  br ");
        printTipo(aux->data.ter->type);
        printf(" %%%d",varNum-1);
      }
      else{
        printf("  br ");
        printTipo(aux->data.ter->type);
        printf(" %s\n",aux->data.ter->name);
      }
      printf(", label %%forfunc%d, label %%contfor%d\n",forNum,forNum);
      aux=aux->data.ter->next;
      break;
  }
  printf("forfunc%d:\n",forNum);
  genFuncBodyMain(aux->data.mult->son);
  printf("  br label %%for%d\n", forNum);
  printf("contfor%d:\n",forNum);
  forNum++;
  return 0;
}

int getIf(no* node){
  no* aux=node->data.mult->son;
  int i=0;
  switch(aux->type){
    case multi:
      i=genFuncBody(aux);
      printf("  br i1 %%%d, label %%if%d, label %%else%d\n",i,ifNum,ifNum);
      aux=aux->data.mult->next;
      break;
    case terminal:
      if(aux->data.ter->tipo==id){
        printf("  %%%d = load ",varNum);
        varNum++;
        printTipo(aux->data.ter->type);
        printf(", ");
        printTipo(aux->data.ter->type);
        printf("* ");
        if(aux->data.ter->isGlobal==1){
          printf(" @%s\n",aux->data.ter->name);
        }
        else{
          printf(" %%%s\n",aux->data.ter->name);
        }
        printf("  br ");
        printTipo(aux->data.ter->type);
        printf(" %%%d",varNum-1);
      }
      else{
        printf("  br ");
        printTipo(aux->data.ter->type);
        printf(" %s\n",aux->data.ter->name);
      }
      printf(", label %%if%d, label %%else%d\n",ifNum,ifNum);
      aux=aux->data.ter->next;
      break;
  }
  printf("if%d:\n",ifNum);
  genFuncBodyMain(aux->data.mult->son);
  printf("  br label %%cont%d\n",ifNum);
  aux=aux->data.mult->next;
  printf("else%d:\n",ifNum);
  genFuncBodyMain(aux->data.mult->son);
  printf("  br label %%cont%d\n",ifNum);
  printf("cont%d:\n",ifNum);
  ifNum++;
  return 0;
}

int getReturn(no* node){
  if(node->data.mult->son==NULL){
    varNum++;
    if(in_main==1){
      printf("  ret i32 0\n");
      return 0;
    }
    printf("  ret void\n");
    return 0;
  }
  int i=0;
  no* aux=node->data.mult->son;
  switch(aux->type){
    case multi:
      i=genFuncBody(aux);
      printf("  ret ");
      printTipo(aux->data.mult->type);
      printf(" %%%d\n",i);
      break;
    case terminal:
      if(aux->data.ter->tipo==id){
        if(aux->data.ter->stored==1){
          printf("  %%%d = load ",varNum);
          varNum++;
          printTipo(aux->data.ter->type);
          printf(", ");
          printTipo(aux->data.ter->type);
          printf("* ");
          if(aux->data.ter->isGlobal==1){
            printf(" @%s\n",aux->data.ter->name);
          }
          else{
            printf(" %%%s\n",aux->data.ter->name);
          }
          printf("  ret ");
          printTipo(aux->data.ter->type);
          printf(" %%%d",varNum-1);
        }
        else{
          printf("  ret ");
          printTipo(aux->data.ter->type);
          if(aux->data.ter->tipo==id){
            if(aux->data.ter->isGlobal==1){
              printf(" @%s\n",aux->data.ter->name);
            }
            else{
              printf(" %%%s\n",aux->data.ter->name);
            }
          }
          else{
            printf(" %s\n",aux->data.ter->name);
          }
        }
      }
      else{
        printf("  ret ");
        printTipo(aux->data.ter->type);
        printf(" %s\n",aux->data.ter->name);
      }
      break;
  }
  varNum++;
  return 0;
}

int getPrint(no* node){
  //call i32(i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str, i32 0, i32 0), i32 %4)
  no* aux= node->data.mult->son;
  int i;
  switch(aux->type){
    case multi:
      i=genFuncBody(aux);
      switch(aux->data.mult->type){
          case 1:
            printf("  call i32(i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.print_int, i32 0, i32 0), i32 %%%d)\n",i);
            break;
          case 2:
            printf("  call void @.print_bool(i1 %%%d)\n",i);
            break;
          case 3:
            printf("  %%%d = fpext float %%%d to double\n",varNum,i);
            varNum++;
            printf("  call i32(i8*, ...) @printf(i8* getelementptr inbounds ([7 x i8], [7 x i8]* @.print_float, i32 0, i32 0), double %%%d)\n",varNum-1);
            break;
          case 4:
            printf("  call i32(i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.print_str, i32 0, i32 0), i8* %%%d)\n",i);
            break;
      }
      break;
    case terminal:
      if(aux->data.ter->tipo==id){
        printf("  %%%d = load ",varNum);
        varNum++;
        printTipo(aux->data.ter->type);
        printf(", ");
        printTipo(aux->data.ter->type);
        printf("* ");
        if(aux->data.ter->isGlobal==1){
          printf("@%s\n",aux->data.ter->name);
        }
        else{
          printf("%%%s\n",aux->data.ter->name);
        }
        if(aux->data.ter->type==3){
          printf("  %%%d = fpext float %%%d to double\n",varNum,varNum -1);
          varNum++;
        }
      }
      switch(aux->data.ter->type){
        case 1:
          printf("  call i32(i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.print_int, i32 0, i32 0), i32 ");
          break;
        case 2:
          printf("  call void @.print_bool(i1 ");
          break;
        case 3:
          printf("  call i32(i8*, ...) @printf(i8* getelementptr inbounds ([7 x i8], [7 x i8]* @.print_float, i32 0, i32 0),double ");
          break;
        case 4:
          printf("  call i32(i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.print_str, i32 0, i32 0), i8* getelementptr ([%ld x i8], [%ld x i8]* @.str.%d, i64 0, i64 0))\n",strlen(aux->data.ter->name),strlen(aux->data.ter->name),str_num);
          str_num++;
          varNum++;
          return 0;
          break;
      }
      if(aux->data.ter->tipo==id){
        printf("%%%d",varNum-1);
      }
      else{
        printf("%s",aux->data.ter->name);
      }
      printf(")");
      printf("\n");
      break;
  }
  varNum++;
  return 0;
}

int getAssign(no* node){
  no* aux=node->data.mult->son;
  no* aux2;
  int i=0;
  int e=0;
  switch(aux->type){
    case multi:
      e=genFuncBody(aux);
      aux2=aux->data.mult->next;
      break;
    case terminal:
      aux2=aux->data.ter->next;
      break;
  }
  aux->data.ter->stored=1;
  setStored(node,aux->data.ter->name);
  switch(aux2->type){
    case multi:
      i=genFuncBody(aux2);
      break;
    case terminal:
      if(aux2->data.ter->tipo==id){
        if(aux2->data.ter->stored==1){
          printf("  %%%d = load ",varNum);
          i=varNum;
          varNum++;
          printTipo(aux2->data.ter->type);
          printf(", ");
          printTipo(aux2->data.ter->type);
          printf("* ");
          if(aux2->data.ter->isGlobal==1){
            printf("@%s\n",aux2->data.ter->name);
          }
          else{
            printf("%%%s\n",aux2->data.ter->name);
          }
        }
      }
      break;
  }

  printf("  store ");
  printTipo(node->data.mult->type);
  if(i!=0){
    printf(" %%%d,",i);
  }
  else{
    if(aux2->data.ter->tipo==id){
      if(aux2->data.ter->isGlobal==1){
        printf(" @%s,",aux2->data.ter->name);
      }
      else{
        printf(" %%%s,",aux2->data.ter->name);
      }
    }
    else{
      printf(" %s,",aux2->data.ter->name);
    }
  }
  printTipo(node->data.mult->type);
  printf("*");
  if(e!=0){
    printf(" %%%d\n",e);
  }
  else{
    if(aux->data.ter->tipo==id){
      if(aux->data.ter->isGlobal==1){
        printf(" @%s\n",aux->data.ter->name);
      }
      else{
        printf(" %%%s\n",aux->data.ter->name);
      }
    }
    else{
      printf(" %s\n",aux->data.ter->name);
    }
  }
  return 0;
}

void setStored(no* n,char* name){
  if (n == NULL) return;
  switch(n->type){
    case multi:
      setStored(n->data.mult->son,name);
      setStored(n->data.mult->next,name);
      break;
    case terminal:
      if(strcmp(n->data.ter->name,name)==0){
        n->data.ter->stored=1;
      }
      setStored(n->data.ter->next,name);
      break;
  }
}
