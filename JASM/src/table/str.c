#include "str.h"
int
/**
 * 添加新字符串入字符串表中
 * @param pList
 * @param ptrString
 * @return
 */
addString(linkedList *pList, char *ptrString)
{
    //--- 检查字符串表中是否已有一样的字符串
    linkedListNode *pNode;
    int iCurrNode;
    pNode = pList->pHead;

    for (iCurrNode = 0; iCurrNode < pList->iNodeCount; iCurrNode++)
    {
        //若一样, 返回此节点的索引, 否则移动到下一个节点
        if (strcmp((char *) pNode->pData, ptrString) == 0)
            return iCurrNode;
        pNode = pNode->pNext;
    }

    //--- 添加新节点
    char *ptrStringNode = (char *) malloc(strlen(ptrString) + 1);
    strcpy(ptrStringNode, ptrString);

    return addNode(pList, ptrStringNode);
}