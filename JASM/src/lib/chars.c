#include "unit.h"

void
//转换大写
charupr(char *ptrChar)
{
    if (isCharLower(*ptrChar))
        *ptrChar -= 32;
}

//是否为数字
Boolean
isCharNumeric(const char cChar)
{
    if (cChar >= '0' && cChar <='9')
        return true;
    return false;
}

//是否为大小写字符
Boolean
isCharLower(const char cChar)
{
    if (cChar >= 'a' && cChar <= 'z')
        return true;
    return false;
}
Boolean
isCharUpper(const char cChar)
{
    if (cChar >= 'A' && cChar <= 'Z')
        return true;
    return false;
}

//是否为空白字符@注: 换行符号不算做空白符
Boolean
isCharWhitespace(const char cChar)
{
    if (cChar == ' ' || cChar == '\t')
        return true;
    return false;
}


//是否为标识符
Boolean
isCharIdent(const char cChar)
{
    if (isCharNumeric(cChar) || isCharLower(cChar)   ||
        isCharUpper(cChar)   || cChar == '_')
        return true;
    return false;
}

//是否为分隔符
Boolean
isCharDelimiter(const char cChar)
{
    if (cChar == ':' || cChar == ',' || cChar == '"' ||
        cChar == '[' || cChar == ']' ||
        cChar == '{' || cChar == '}' ||
        isCharWhitespace(cChar) || cChar == '\n')
        return true;
    return false;
}