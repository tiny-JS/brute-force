#include "instr.h"
#include "../common/link.h"

int
/**
 * 添加新指令
 * @param pList
 * @param ptrMnemonic
 * @param iOpcode
 * @param iOpCount
 * @return
 */
addInstrLookup(linkedList *pList, char *ptrMnemonic, int iOpcode, int iOpCount)
{
    int  iIndex;
    instrLookup *pNewInstr = (instrLookup *) malloc(sizeof(instrLookup));
    pNewInstr->ptrMnemonic = ptrMnemonic;
    pNewInstr->iOpcode  = iOpcode;
    pNewInstr->iOpCount = iOpCount;
    pNewInstr->opList   = (opTypes *) malloc(iOpCount * sizeof(opTypes));

    iIndex = addNode(pList, pNewInstr);
    pNewInstr->iIndex = iIndex;

    return iIndex;
}

instrLookup
/**
 * 获取指令表中给定索引的指令
 * @param pList
 * @param iIndex
 * @return
 */
*getInstrByIndex(linkedList *pList, int iIndex)
{
    linkedListNode *pCurrNode;
    instrLookup *pInstr;
    pCurrNode = pList->pHead;
    if (!pList->iNodeCount)
        return NULL;

    while (pCurrNode)
    {
        pInstr = (instrLookup *) pCurrNode->pData;
        if (pInstr->iIndex == iIndex)
            return pInstr;
        pCurrNode = pCurrNode->pNext;
    }

    return NULL;
}

void
/**
 * 设置操作数
 * @param pList
 * @param iInStrIndex
 * @param iOpIndex
 * @param iOpType
 */
setOpType(linkedList *pList, int iInStrIndex, int iOpIndex, opTypes iOpType)
{
    instrLookup *pInstr;
    pInstr = getInstrByIndex(pList, iInStrIndex);

    if (pInstr)
        pInstr->opList[iOpIndex] = iOpType;
}

instrLookup
*getInstrByMnemonic(linkedList *pList, char *ptrMnemonic)
{
    linkedListNode *pNode = pList->pHead;
    instrLookup *pInstrNode;
    if (!pNode)
        return NULL;
    while (pNode)
    {
        pInstrNode = (instrLookup *) pNode->pData;
        if (!strcasecmp(pInstrNode->ptrMnemonic, ptrMnemonic))
            return pInstrNode;
        pNode = pNode->pNext;
    }
    return NULL;
}
