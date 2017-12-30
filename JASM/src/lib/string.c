#include "unit.h"

void
//转换为大写字符串
strupr(char *ptrString)
{
    size_t len;
    size_t index;
    len = strlen(ptrString);

    for (index = 0; index < len; index++)
        charupr(&ptrString[index]);
}

Boolean
/**
 * 是否为字符串数字
 * @param ptrString
 * @return
 */
isStringInt(const char *ptrString)
{
    uint iCurrCharIndex;
    size_t len;

    if (!ptrString || (len = strlen(ptrString)) == 0)
        return false;

    //# 检测是否为数字或者负号
    for (iCurrCharIndex = 0; iCurrCharIndex < len; iCurrCharIndex++)
        if (!isCharNumeric(ptrString[iCurrCharIndex]) && ptrString[iCurrCharIndex] != '-')
            return false;

    //# 检测负号是否不在数字开头
    for (iCurrCharIndex = 1;  iCurrCharIndex < len ; iCurrCharIndex++)
        if (ptrString[iCurrCharIndex] == '-')
            return false;

    return true;
}

Boolean
/**
 * 是否为浮点型字符串
 * @param ptrString
 * @return
 */
isStringFloat(const char *ptrString)
{
    uint iCurrCharIndex;
    Boolean iRadixPointFound;
    size_t len;
    len = strlen(ptrString);
    iRadixPointFound = false;

    if (!ptrString || len == 0)
        return false;

    //# 字符串是否为数字小数点以及负号组成
    for (iCurrCharIndex = 0; iCurrCharIndex < len; iCurrCharIndex++)
        if (! isCharNumeric(ptrString[iCurrCharIndex]) &&
            ! (ptrString[iCurrCharIndex]) == '.' &&
            ! (ptrString[iCurrCharIndex]) == '-')
            return false;

    //# 字符串只有一个小数点
    for (iCurrCharIndex = 0; iCurrCharIndex < len; iCurrCharIndex++)
        if (ptrString[iCurrCharIndex] == '.')
            if (iRadixPointFound)
                return false;
            else
                iRadixPointFound = true;

    //# 字符串只有一个负号 @注: 并且在首字符
    for (iCurrCharIndex = 1; iCurrCharIndex < len; iCurrCharIndex++)
        if (ptrString[iCurrCharIndex] == '-')
            return false;

    //# 字符串无小数点
    if (!iRadixPointFound)
        return false;

    return true;
}

Boolean
/**
 * 是否为空字符串
 * @param ptrString
 * @return
 */
isStringWhitespace(const char *ptrString)
{
    uint iCurrCharIndex;
    size_t len;
    len = strlen(ptrString);

    if (!ptrString)
        return false;

    for (iCurrCharIndex = 0; iCurrCharIndex < len; iCurrCharIndex++)
        if (!isCharWhitespace(ptrString[iCurrCharIndex]))
            return false;

    return true;
}

Boolean
/**
 * 字符串是否为标识符 @注: 标识符首字符不可数字
 * @param ptrString
 * @return
 */
isStringIdent(const char *ptrString)
{
    uint iCurrCharIndex;
    size_t len;
    len = strlen(ptrString);

    if (!ptrString || len == 0)
        return false;

    //# 标识符首字符不可为数字
    if (isCharNumeric(ptrString[0]))
        return false;

    for (iCurrCharIndex = 0; iCurrCharIndex < len; iCurrCharIndex++)
        if (!isCharIdent(ptrString[iCurrCharIndex]))
            return false;
    return true;
}