#ifndef TINYJS_LABELTABLE_H
#define TINYJS_LABELTABLE_H

#include "../common/JASM.h"
//# label table

//>相关函数
int addLabel(linkedList *pList, char *ptrIdent, int iTargetIndex, int iFunction);
labelNode *getLabelByIdent(linkedList *pList, char *ptrIdent, int iFuncIndex);

#endif //TINYJS_LABELTABLE_H
