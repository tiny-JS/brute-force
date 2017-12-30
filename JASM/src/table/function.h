#ifndef TINYJS_FUNCTIONTABLE_H
#define TINYJS_FUNCTIONTABLE_H

#include "../common/JASM.h"

//> 相关函数
//添加新函数入函数表中, 函数若已经存在返回-1, 否则返回索引
int addFunc(linkedList *pList, char *ptrName, int iEntryPoint);
//设置函数的局部信息
void setFuncInfo(linkedList *pList, char *ptrName, int iParamCount, int iLocalDataSize);
//根据函数的名字获取函数索引
funcNode *getFuncByName(linkedList *pList, char *ptrName);

#endif //TINYJS_FUNCTIONTABLE_H
