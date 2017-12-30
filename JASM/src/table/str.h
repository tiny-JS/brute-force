#ifndef TINYJS_STRINGTABLE_H
#define TINYJS_STRINGTABLE_H

#include "../common/JASM.h"
//# string table
// <空结构>
//> 相关函数
//添加新的字符串入字符串表中, 返回新字符串节点的索引
int addString(linkedList *pList, char *ptrString);

#endif //TINYJS_STRINGTABLE_H
