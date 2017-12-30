#ifndef TINYJS_UNIT_H
#define TINYJS_UNIT_H

#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "../common/JASM.h"


#define MAXLINE 8192
/**
 * * * * * * * * * * * * * * * * * * * * * * *
 * 字符处理系列函数                            |
 * * * * * * * * * * * * * * * * * * * * * * *
 */

//转换字符为大写
extern void charupr(char *ptrChar);

// 判断一个字符是否是数字字符
extern Boolean isCharNumeric(const char cChar);

//判断一个字符是否为小/小写字母
extern Boolean isCharLower(const char cChar);
extern Boolean isCharUpper(const char cChar);

//判断一个字符是否是空白符
extern Boolean isCharWhitespace(const char cChar);

//判断一个字符是否是有效标识符的部分
extern Boolean isCharIdent(const char cChar);

//判断一个字符是否是分隔符的部分
extern Boolean isCharDelimiter(const char cChar);

/**
 * * * * * * * * * * * * * * * * * * * * * * *
 * 字符串处理系列函数                           |
 * * * * * * * * * * * * * * * * * * * * * * *
 */

//是否为数字字符串 @注: 整型不包括浮点型
extern Boolean isStringInt(const char *ptrString);

//是否为浮点字符串
extern Boolean isStringFloat(const char *ptrString);

//是否为空白字符串
extern Boolean isStringWhitespace(const char *ptrString);

//字符串是否为标识符 @注: 标识符首字符不可数字
extern Boolean isStringIdent(const char *ptrString);

//转换为大写字符串
extern void strupr(char *ptrString);

/**
 **********************************************
 * 错误处理                                    #
 **********************************************
 */
extern void exitOnCharExpectedError(char cChar);

extern void err_sys(const char *fmt, ...);

extern off_t getFileSize(const char *ptrName);

#endif //TINYJS_UNIT_H
