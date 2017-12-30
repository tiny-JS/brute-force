#include "symbol.h"
#include "hostAPI.h"

int
/**
 * 添加新符号入符号表中
 * @param pList
 * @param ptrIdent
 * @param iSize
 * @param iStackIndex
 * @param iFuncIndex
 * @return
 */
addSymbol(linkedList *pList, char *ptrIdent, int iSize, int iStackIndex, int iFuncIndex)
{
    //标识符已经存在
    if (getSymbolByIdent(pList, ptrIdent, iFuncIndex))
        return -1;

    symbolNode *pNewSymbol;
    int iIndex;
    pNewSymbol = (symbolNode *) malloc(sizeof(symbolNode));
    pNewSymbol->ptrIdent = (char *) malloc(strlen(ptrIdent));
    pNewSymbol->ptrIdent = ptrIdent;
    pNewSymbol->iSize = iSize;
    pNewSymbol->iStackIndex = iStackIndex;
    pNewSymbol->iFuncIndex  = iFuncIndex;

    iIndex = addNode(pList, pNewSymbol);
    pNewSymbol->iIndex = iIndex;

    return iIndex;
}

symbolNode
/**
 * 根据标识符名称和所声明的函数索引标识, 来返回标识符信息 @注: 可查全局
 * @param pList
 * @param ptrIdent
 * @param iFuncIndex
 * @return
 */
*getSymbolByIdent(linkedList *pList, char *ptrIdent, int iFuncIndex)
{

    if (!pList->iNodeCount)
        return NULL;

    linkedListNode *pCurrNode;
    symbolNode *pCurrSymbolNode;
    size_t iCurrIndex;
    pCurrNode = pList->pHead;
    for (iCurrIndex = 0; iCurrIndex < pList->iNodeCount; iCurrIndex++)
    {
        pCurrSymbolNode = (symbolNode *) pCurrNode->pData;
        if (!strcmp(pCurrSymbolNode->ptrIdent, ptrIdent))
            //全局标识符或者是某个函数的局部标识符
            if (pCurrSymbolNode->iFuncIndex == iFuncIndex || pCurrSymbolNode->iStackIndex >= 0)
                return pCurrSymbolNode;

        pCurrNode = pCurrNode->pNext;
    }
    return NULL;
}

int
/**
 * 获取标识符的堆栈索引
 * @param pList
 * @param ptrIdent
 * @param iFuncIndex
 * @return
 */
getStackIndexByIdent(linkedList *pList, char *ptrIdent, int iFuncIndex)
{
    symbolNode *pSymbolNode;
    pSymbolNode = getSymbolByIdent(pList, ptrIdent, iFuncIndex);

    return pSymbolNode->iStackIndex;
}

int
/**
 * 返回变量大小
 * @param pList
 * @param ptrIdent
 * @param iFuncIndex
 * @return
 */
getSizeByIdent(linkedList *pList, char *ptrIdent, int iFuncIndex)
{
    symbolNode *pSymbolNode;
    pSymbolNode = getSymbolByIdent(pList, ptrIdent, iFuncIndex);

    return pSymbolNode->iSize;
}
