#ifndef TINYJS_INSTR_H
#define TINYJS_INSTR_H

#include "../common/JASM.h"


//> 相关函数
int addInstrLookup(linkedList *pList, char *ptrMnemonic, int iOpcode, int iOpCount);
instrLookup *getInstrByIndex(linkedList *pList, int iIndex);
instrLookup *getInstrByMnemonic(linkedList *pList, char *ptrMnemonic);
void setOpType(linkedList *pList, int iInStrIndex, int iOpIndex, opTypes iOpType);

#endif //TINYJS_INSTR_H
