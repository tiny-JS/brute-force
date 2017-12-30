#ifndef TINYJS_LINK_H
#define TINYJS_LINK_H

#include "JASM.h"

//---------------- 链表相关操作 ---------------------
//初始化链表
void initLinkedList(linkedList *pList);

//新增节点
int addNode(linkedList *pList, void *pData);

//释放链表
void freeLinkedList(linkedList *pList);

#endif //TINYJS_LINK_H
