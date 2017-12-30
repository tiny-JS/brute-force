#include "function.h"

int
/**
 * 添加新函数
 * @param pList
 * @param ptrName
 * @param iEntryPoint 函数入口点
 * @return
 */
addFunc(linkedList *pList, char *ptrName, int iEntryPoint)
{
    int iIndex;
    if (getFuncByName(pList, ptrName))
        return -1;
    funcNode *pNewFunc = (funcNode *) malloc(sizeof(funcNode));
    pNewFunc->ptrName = (char *) malloc(strlen(ptrName));
    strcpy(pNewFunc->ptrName, ptrName);
    pNewFunc->iEntryPoint = iEntryPoint;
    iIndex = addNode(pList, pNewFunc);
    pNewFunc->iIndex = iIndex;

    return iIndex;
}

void
/**
 * 设置函数局部信息 @注: 如参数和局部数据大小
 * @param pList
 * @param ptrName
 * @param iParamCount
 * @param iLocalDataSize
 */
setFuncInfo(linkedList *pList, char *ptrName, int iParamCount, int iLocalDataSize)
{
    funcNode *pFunc;
    pFunc = getFuncByName(pList, ptrName);

    pFunc->iParamCount = iParamCount;
    pFunc->iLocalDataSize = iLocalDataSize;
}

funcNode
/**
 * 根据函数名字在函数表中获取函数节点
 * @param pList
 * @param ptrName
 * @return
 */
*getFuncByName(linkedList *pList, char *ptrName)
{
    linkedListNode *pCurrNode;
    funcNode *pCurrFunc;
    size_t iCurrNode;
    pCurrNode = pList->pHead;

    if (!pList->iNodeCount)
        return NULL;

    for (iCurrNode = 0; iCurrNode < pList->iNodeCount; iCurrNode++) {
        pCurrFunc = (funcNode *) pCurrNode->pData;

        if (!strcmp(pCurrFunc->ptrName, ptrName))
            return pCurrFunc;

        pCurrNode = pCurrNode->pNext;
    }

    return NULL;
}