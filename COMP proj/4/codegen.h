void genCode(no* tree);
void genNode(no* no);
void printTipo(int n);
void genVar(no* no);
void genFunc(no* no);
void genFuncDecl(no* no);
void getFuncParams(no* no);
void genFuncBodyMain(no* node);
int genFuncBody(no* node);
int getVarDecl(no* no);
int getOr(no* no);
int getAnd(no* no);
int getEq(no* no);
int getNe(no* no);
int getLt(no* no);
int getGt(no* no);
int getLe(no* no);
int getGe(no* no);
int getAdd(no* no);
int getSub(no* no);
int getMul(no* no);
int getDiv(no* no);
int getMod(no* no);
int getNot(no* no);
int getMinus(no* no);
int getPlus(no* no);
int getPlus(no* no);
int getOr(no* no);
int getCall(no* no);
int getParseArgs(no* no);
int getFor(no* no);
int getIf(no* no);
int getReturn(no* no);
int getPrint(no* no);
int getAssign(no* no);
void genStr(no* n);
void setStored(no* n,char* name);
void strprint(no* n);
