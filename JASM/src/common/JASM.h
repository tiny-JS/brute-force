/**
 * JASM - The tinyJs Assembler Version 0.1
 * 生成JVM可执行的指令流
 */
#ifndef TINYJS_JASM_H
#define TINYJS_JASM_H

//----Include Files-----
#include <stdio.h>


#include "define.h"
#include "link.h"
#include "../lib/unit.h"
#include "../table/str.h"
#include "../table/function.h"
#include "../table/symbol.h"
#include "../table/label.h"
#include "../table/hostAPI.h"
#include "../table/instr.h"
#include "../lexer/lexer.h"
#include "../parser/parser.h"
#include "../print/print.h"
#include "../init/init.h"


//----------------------
//| main header        |
//| instruction Stream |
//| string table       |
//| function table     |
//| host api call table|
//----------------------

//-------- 可执行文件层次结构定义 @注: 参考asm.output.txt详情, 与.xse文件层次相同顺序 ------------------

#endif //COMPILE_JASM_H
