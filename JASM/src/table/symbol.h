#ifndef TINYJS_SYMBOLTABLE_H
#define TINYJS_SYMBOLTABLE_H

#include "../common/JASM.h"

//# symbol table

//>相关函数
//添加新符号, 会返回索引
int addSymbol(linkedList *pList, char *ptrIdent, int iSize, int iStackIndex, int iFuncIndex);
//根据标识符获取符号表信息
symbolNode *getSymbolByIdent(linkedList *pList, char *ptrIdent, int iFuncIndex);
//根据标识符和所在函数索引返回其堆栈索引
int getStackIndexByIdent(linkedList *pList, char *ptrIdent, int iFuncIndex);
//返回标识符大小
int getSizeByIdent(linkedList *pList, char *ptrIdent, int iFuncIndex);


#endif //TINYJS_SYMBOLTABLE_H
