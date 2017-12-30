#include "link.h"
void
/**
 * 初始化链表
 * @param pList
 */
initLinkedList(linkedList *pList)
{
    //置空头尾节点指针
    pList->pHead = NULL;
    pList->pTail = NULL;

    pList->iNodeCount = 0;
}

int
/**
 * 添加新节点, 返回新节点插入链表后的索引
 * @param pList
 * @param pData
 * @return
 */
addNode(linkedList *pList, void *pData)
{
    linkedListNode *pNewNode;
    pNewNode = (linkedListNode *) malloc(sizeof(linkedListNode));

    pNewNode->pData = pData;
    pNewNode->pNext = NULL;

    //-更新链表头尾指针和结点统计
    if (!pList->iNodeCount)
    {
        pList->pHead = pNewNode;
        pList->pTail = pNewNode;
    }
    else
    {
        pList->pTail->pNext = pNewNode;
        pList->pTail        = pNewNode;
    }
    ++ pList->iNodeCount;

    //返回新节点在链表中的索引
    return pList->iNodeCount - 1;
}

void
/**
 * 释放链表
 * @param pList
 */
freelinkedList(linkedList *pList)
{
    if (!pList)
        return;

    if (pList->iNodeCount)
    {
        linkedListNode *pCurrNode,
                       *pNextNode;
        pCurrNode = pList->pHead;

        while (1)
        {
            pNextNode = pCurrNode->pNext;
            //清楚节点数据
            if (pCurrNode->pData)
                free(pCurrNode->pData);
            //清除节点
            if (pCurrNode)
                free(pCurrNode);

            if (pNextNode)
                pCurrNode = pNextNode;
            else
                break;
        }
    }
}