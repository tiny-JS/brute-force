#ifndef TINYJS_PRINT_H
#define TINYJS_PRINT_H

#include "../common/JASM.h"
//显示编译器信息
void printLogo(global *GLOBAL);
//编译器选项提示
void printUsage();
//编译器错误提示

//编译统计信息
void printAssmblStats(global *GLOBAL, state *info);

#endif
