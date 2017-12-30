#include "label.h"
int
/**
 * 添加新标签
 * @param pList
 * @param ptrIdent
 * @param iTargetIndex
 * @param iFuncIndex
 * @return
 */
addLabel(linkedList *pList, char *ptrIdent, int iTargetIndex, int iFuncIndex)
{
    if (getLabelByIdent(pList, ptrIdent, iFuncIndex))
        return -1;

    int iIndex;

    labelNode *pNewLabel = (labelNode *) malloc(sizeof(labelNode));
    pNewLabel->ptrIdent  = (char *) malloc(sizeof(ptrIdent));
    pNewLabel->iTargetIndex = iTargetIndex;
    pNewLabel->iFuncIndex   = iFuncIndex;

    iIndex = addNode(pList, pNewLabel);

    pNewLabel->iIndex = iIndex;

    return iIndex;
}

labelNode
/**
 * 根据标识符和声明所在函数索引查找
 * @param pList
 * @param ptrIdent
 * @param iFuncIndex
 * @return
 */
*getLabelByIdent(linkedList *pList, char *ptrIdent, int iFuncIndex)
{
    if (!pList->iNodeCount)
        return NULL;

    linkedListNode *pCurrNode;
    labelNode *pCurrLabel;
    int iCurrNode;
    pCurrNode = pList->pHead;

    for (iCurrNode = 0; iCurrNode < pList->iNodeCount; iCurrNode++)
    {
        pCurrLabel = (labelNode *) pCurrNode->pData;

        if (!strcmp(pCurrLabel->ptrIdent, ptrIdent) &&
            pCurrLabel->iFuncIndex == iFuncIndex)
            return pCurrLabel;

        pCurrNode = pCurrNode->pNext;
    }

    return NULL;
}