#include "lexer.h"

char
/**
 * 复制当前匹配单词, 避免被下次属性字查找导致内存被释放
 * @param lexer
 * @return
 */
*getCurrLexeme(lexer *lexer)
{
    size_t tmpSize = strlen(lexer->ptrCurrLexeme)+1;
    char *ptrTmp = strcpy((char *) malloc(tmpSize), lexer->ptrCurrLexeme);
    ptrTmp[tmpSize-1] = '\0';
    return ptrTmp;

}

void
/**
 * 属性字匹配
 * 匹配setStackSize, func, 指令, 变量, 字面值
 * @param lexer
 * @param GLOBAL
 * @param info
 */
tokenMatch(lexer *lexer, global *GLOBAL, state *info)
{
    while (true)
    {
        if (getNextToken(lexer, GLOBAL) == END_OF_TOKEN_STREAM)
            break;
        switch (lexer->currToken)
        {
            //$ 设置椎栈大小
            case TOKEN_TYPE_SETSTACKSIZE:
            {
                //> 椎栈设置不允许出现在函数中
                if (info->iIsFuncActive)
                    err_sys(ERROR_MSSG_LOCAL_SETSTACKSIZE);

                //> 只允许设置一次
                if (info->iIsSetStackSizeFound)
                    err_sys(ERROR_MSSG_MULTIPLE_SETSTACKSIZES);

                //> 仅允许为整数字面值
                if (getNextToken(lexer, GLOBAL) != TOKEN_TYPE_INT)
                    err_sys(ERROR_MSSG_INVALID_STACK_SIZE);

                //> 设置头部信息椎栈大小
                GLOBAL->headerInfo.iStackSize = atoi(lexer->ptrCurrLexeme);
                //> 状态改变
                info->iIsSetStackSizeFound = true;
                break;
            }

            //$ 变量声明 var / var[]
            case TOKEN_TYPE_VAR:
            {
                //> 变量声明第一个必须是标识符
                if (getNextToken(lexer, GLOBAL) != TOKEN_TYPE_IDENT)
                    err_sys(ERROR_MSSG_IDENT_EXPECTED);

                int iSize = 0;
//                printf("%s\n", lexer->ptrCurrLexeme);
                char *ptrIdent = getCurrLexeme(lexer);
                int iStackIndex;

                //> 查看是数组还是单独的变量
                if (getLookAheadChar(lexer, GLOBAL) == '[')
                {
                    //> 是数组就将数组大小为此变量占据的全局/局部大小
                    if (getNextToken(lexer, GLOBAL) != TOKEN_TYPE_OPEN_BRACKET)
                        exitOnCharExpectedError('[');

                    //> 数组大小仅仅只能是整数字面值, 不可为负
                    if (getNextToken(lexer, GLOBAL) != TOKEN_TYPE_INT)
                        err_sys(ERROR_MSSG_INVALID_ARRAY_SIZE);

                    iSize = atoi(lexer->ptrCurrLexeme);
                    if (iSize <= 0)
                        err_sys(ERROR_MSSG_INVALID_ARRAY_SIZE);

                    if (getNextToken(lexer, GLOBAL) != TOKEN_TYPE_CLOSE_BRACKET)
                        exitOnCharExpectedError(']');

                }
                //> 椎栈设置, 在函数中声明为负数索引, 在全局为正数索引
                if (info->iIsFuncActive)
                    iStackIndex = -(info->iCurrFuncLocalDataSize+2);
                else
                    iStackIndex = GLOBAL->headerInfo.iGlobalDateSize;

                //> 将变量名和信息添加入符号表
                if (addSymbol(GLOBAL->symbolTable, ptrIdent, iSize, iStackIndex, info->iCurrFuncIndex) == -1)
                    err_sys(ERROR_MSSG_IDENT_REDEFINITION);

                if (info->iIsFuncActive)
                    info->iCurrFuncLocalDataSize += iSize;
                else
                    GLOBAL->headerInfo.iGlobalDateSize += iSize;

                break;
            }

                //$ 函数
            case TOKEN_TYPE_FUNC:
            {
                //> 不可内嵌函数
                if (info->iIsFuncActive)
                    err_sys(ERROR_MSSG_NESTED_FUNC);

                //> func关键字后是标识符号
                if (getNextToken(lexer, GLOBAL) != TOKEN_TYPE_IDENT)
                    err_sys(ERROR_MSSG_IDENT_EXPECTED);

                //> 获取函数名称
                char *ptrName = getCurrLexeme(lexer);
                //> 计算函数入口点
                int iEntryPoint = info->iInstrStreamSize;

                //> 添加函数
                int iFuncIndex = addFunc(GLOBAL->funcTable, ptrName, iEntryPoint);

                if (iFuncIndex == -1)
                    err_sys(ERROR_MSSG_FUNC_REDEFINITION);

                //> 当前函数是否为特殊_main函数
                if ( strcasecmp (ptrName, MAIN_FUNC_NAME) == 0 )
                {
                    GLOBAL->headerInfo.iMainFuncIndex = iFuncIndex;
                    GLOBAL->headerInfo.iIsMainFuncPresent = true;
                }

                //> 设置状态, 以便于语法分析识别局部/全局
                info->iIsFuncActive = true;
                info->ptrCurrFuncName = ptrName;
                info->iCurrFuncIndex  = iFuncIndex;
                info->iCurrFuncParamCount = info->iCurrFuncLocalDataSize = 0;

                //> 函数名字标识符后可以跟着无论多少换行、空白
                while (getNextToken(lexer, GLOBAL) == TOKEN_TYPE_NEWLINE) ;

                //> 是否到左大括号
                if (lexer->currToken != TOKEN_TYPE_OPEN_BRACE)
                    exitOnCharExpectedError('{');

                ++info->iInstrStreamSize;
                break;
            }

                //$ 函数右大括号
            case TOKEN_TYPE_CLOSE_BRACE:
            {
                //> 右大括号目前只作为函数结尾
                if (!info->iIsFuncActive)
                    exitOnCharExpectedError('}');

                //> 设置统计完的函数属性
                setFuncInfo(GLOBAL->funcTable, info->ptrCurrFuncName, info->iCurrFuncParamCount, info->iCurrFuncLocalDataSize);
                //> 关闭当前函数状态
                info->iIsFuncActive = false;
                break;
            }

                //$ 函数参数
            case TOKEN_TYPE_PARAM:
            {
                //> 仅只能在函数内部使用
                if (!info->iIsFuncActive)
                    err_sys(ERROR_MSSG_GLOBAL_PARAM);

                //> main函数没有参数
                if (!strcmp(info->ptrCurrFuncName, MAIN_FUNC_NAME))
                    err_sys(ERROR_MSSG_MAIN_PARAM);

                //> param后必须为标识符
                if (getNextToken(lexer, GLOBAL) != TOKEN_TYPE_IDENT)
                    err_sys(ERROR_MSSG_IDENT_EXPECTED);

                ++info->iCurrFuncParamCount;
                break;
            }

                //$ 操作指令
            case TOKEN_TYPE_INSTR:
            {
                //> 指令仅在函数中运行
                if (!info->iIsFuncActive)
                    err_sys(ERROR_MSSG_GLOBAL_INSTR);
                ++info->iInstrStreamSize;
                break;
            }

            //$ label标识符
            case TOKEN_TYPE_IDENT:
            {
                //> label标识符之后需要冒号
                if (getLookAheadChar(lexer, GLOBAL) != ':')
                    err_sys(ERROR_MSSG_INVALID_INSTR);

                //> 函数中才能有label
                if (!info->iIsFuncActive)
                    err_sys(ERROR_MSSG_GLOBAL_LINE_LABEL);

                //> 添加标签表
                char *ptrName = getCurrLexeme(lexer);
                int  iTargetIndex = info->iInstrStreamSize - 1;
                int iFuncIndex    = info->iCurrFuncIndex;
                if (addLabel(GLOBAL->labelTable, ptrName, iTargetIndex, iFuncIndex) == -1)
                    err_sys(ERROR_MSSG_LINE_LABEL_REDEFINITION);

                break;
            }

            default:
                if ( lexer->currToken != TOKEN_TYPE_NEWLINE )
                    err_sys(ERROR_MSSG_INVALID_INPUT);
        }

        //> 分析下一行
        if (!skipToNextLine(lexer, GLOBAL))
            break;
    }
}

char
/**
 * 向前查看 @注: 不改变词法分析器状态
 * @param lexer
 * @param GLOBAL
 * @return
 */
getLookAheadChar(lexer *lexer, global *GLOBAL)
{
    //> 不改变词法分析器状态
    size_t iCurrSourceLine = lexer->iCurrSourceLine;
    size_t iIndex          = lexer->iIndex1;

    //> 词法分析器在字符串状态就直接返回前置值, 否则如果iIndex值已经是一行中的末尾, 将移动到下一行
    //> 并将iIndex移动到不是空白字符的坐标
    if (lexer->iCurrLexState != LEX_STATE_IN_STRING)
    {
        while (true)
        {
            //> 一行的末尾
            if (iIndex >= strlen(GLOBAL->ptrSourceCode[iCurrSourceLine]))
            {
                ++iCurrSourceLine;
                if (iCurrSourceLine >= GLOBAL->iSourceCodeLine)
                    return 0;

                iIndex = 0;
            }

            //> 如果已经
            if (!isCharWhitespace(GLOBAL->ptrSourceCode[iCurrSourceLine][iIndex]))
                break;

            ++iIndex;
        }
    }

    return GLOBAL->ptrSourceCode[iCurrSourceLine][iIndex];
}

Boolean
/**
 * 下跳一行, 除非已经是数据尾部
 * @param lexer
 * @param GLOBAL
 * @return
 */
skipToNextLine(lexer *lexer, global *GLOBAL)
{
    ++lexer->iCurrSourceLine;
    if (lexer->iCurrSourceLine >= GLOBAL->iSourceCodeLine)
        return false;

    lexer->iIndex0 = lexer->iIndex1 = 0;
    lexer->iCurrLexState = LEX_STATE_NO_STRING;
    return true;
}

void
/**
 * 重置词法分析器状态
 * @param lexer
 */
resetLexer(lexer *lexer)
{
    lexer->iCurrSourceLine = lexer->iIndex1 = lexer->iIndex0 = 0;
    lexer->currToken = TOKEN_TYPE_INVALID;
    lexer->iCurrLexState = LEX_STATE_NO_STRING;
}

token
getNextToken(lexer *lexer, global *GLOBAL)
{
    if (lexer->ptrCurrLexeme)
        free(lexer->ptrCurrLexeme);
    //$ 单词提取
    //> 将第一个索引移到最后一个属性字的尾部
    //> 最后一个属性字尾部是用第二个索引进行标识的
    lexer->iIndex0 = lexer->iIndex1;
    //> 确保我们没有扫描到当前行的尾部.
    //> 如果一个字符串有8个字符, 它的索引就是0~7;
    //> 因此,索引8将在字符串的外面并且需要我们移到下一行. 这就是我们为什么使用>=进行比较而不使用>的原因
    if (lexer->iIndex0 >= strlen(GLOBAL->ptrSourceCode[lexer->iCurrSourceLine]))
    {
        //> 如果是, 那就跳到下一行, 但要我们要确保没有超过文件的尾部.
        //> 如果我们到达文件的尾部(属性字流的尾部), toNextLine()将会返回false
        if (!skipToNextLine(lexer, GLOBAL))
            return END_OF_TOKEN_STREAM;
    }
    //> 如果我们刚刚结束了对字符串的分析, 就要停止对字符串的词法分析并返回正常状态
    if (lexer->iCurrLexState == LEX_STATE_END_STRING)
        lexer->iCurrLexState = LEX_STATE_NO_STRING;

    //> 跳过下一个单词前的空白符, 但是这只是对当前处理不是字符串的情况(因为字符串中可以包含任意的空白符, 且需要保留)
    if (lexer->iCurrLexState != LEX_STATE_IN_STRING)
        //> 扫描空白符并检查是否是行的尾部
        while (true)
        {
            //> 如果当前字符不是空白符, 退出循环, 因为单词开始
            if (!isCharWhitespace(GLOBAL->ptrSourceCode[lexer->iCurrSourceLine][lexer->iIndex0]))
                break;

            //> 然而如果是空白符, 就移到下一个字符并继续扫描
            ++lexer->iIndex0;
        }

    //> 把第二个索引指向单词开始的字符, 通过第一个索引进行标识
    lexer->iIndex1 = lexer->iIndex0;

    //> 继续往下扫描直到遇到分隔符, 每次第二个索引都加1
    while (true)
    {
        //> 当前是否在扫描字符串
        if (lexer->iCurrLexState == LEX_STATE_IN_STRING)
        {
            //> 如果位于行的尾部, 返回一个不合法的属性字, 因为在该行中字符串没有双引号
            if (lexer->iIndex1 >= strlen(GLOBAL->ptrSourceCode[lexer->iCurrSourceLine]))
            {
                lexer->currToken = TOKEN_TYPE_INVALID;
                return lexer->currToken;
            }

            //> 如果当前字符是反斜线, 向前移动两个字符来跳过转义字符, 并且跳到下一个循环中
            if (GLOBAL->ptrSourceCode[lexer->iCurrSourceLine][lexer->iIndex1] == '\\')
            {
                lexer->iIndex1 += 2;
                continue;
            }

            //> 如果当前字符不是双引号, 向前移动一个字符, 否则退出循环, 因为字符串已经结束
            if (GLOBAL->ptrSourceCode[lexer->iCurrSourceLine][lexer->iIndex1] == '"')
                break;
            lexer->iIndex1++;
        }
        //> 现在扫描的不是字符串
        else
        {
            //> 如果处于行的尾部, 单词结束所以退出循环
            if (lexer->iIndex1 >= strlen(GLOBAL->ptrSourceCode[lexer->iCurrSourceLine]))
                break;

            //> 如果当前字符不是分隔符, 向前移动一个字符, 否则退出循环
            if (isCharDelimiter(GLOBAL->ptrSourceCode[lexer->iCurrSourceLine][lexer->iIndex1]))
                break;
            lexer->iIndex1++;
        }
    }

    //> 这是单字符单词看起来包含0个字符(因为索引1和索引2相等), 所以把索引2向后移动1个, 这样可以让它有明显的宽度
    if (lexer->iIndex1 == lexer->iIndex0)
        lexer->iIndex1++;

    //> 单词已经被分离出来, 位于索引0和索引1之间, 所以可以词法分析器做一个局部备份
    uint iCurrDestIndex = 0;
    size_t iCurrSourceIndex = lexer->iIndex0;
    lexer->ptrCurrLexeme = (char *) malloc(lexer->iIndex1 - lexer->iIndex0);
//    printf("%d %d %cfdfd\n", lexer->iIndex0, lexer->iIndex1, GLOBAL->ptrSourceCode[lexer->iCurrSourceLine][lexer->iIndex1]);

    for (; iCurrSourceIndex < lexer->iIndex1; iCurrSourceIndex++)
    {
        //> 如果正在分析一个字符串, 检查转义字符并且复制反斜线之后的字符
        if (lexer->iCurrLexState == LEX_STATE_IN_STRING)
            if (GLOBAL->ptrSourceCode[lexer->iCurrSourceLine][iCurrSourceIndex] == '\\')
                iCurrSourceIndex++;

        //> 从源代码行中复制字符到单词中
        lexer->ptrCurrLexeme[iCurrDestIndex] = GLOBAL->ptrSourceCode[lexer->iCurrSourceLine][iCurrSourceIndex];

        //> 修改目标索引
        ++iCurrDestIndex;
    }

    //> 设置NULL终止符
    lexer->ptrCurrLexeme[iCurrDestIndex] = '\0';
    //> 如果不是字符串, 就把字符转变成大写
    if (lexer->iCurrLexState != LEX_STATE_IN_STRING)
        strupr(lexer->ptrCurrLexeme);

    //$ 属性字识别

    //> 找出新的单词是哪种类型的属性字, 如果词法分析器没有相匹配的属性字类型, 就设置为不合法类型
    lexer->currToken = TOKEN_TYPE_INVALID;

    //> 第一种情况最简单的, 如果字符串单词状态是活跃的
    //> 我们就知道处理的是字符串属性字. 然而, 如果字符串是双引号标志
    //> 这就意味着我们读到的是空串, 并且应该返回一个双引号
    if (strlen(lexer->ptrCurrLexeme) > 1 || lexer->ptrCurrLexeme[0] != '"')
    {
        if (lexer->iCurrLexState == LEX_STATE_IN_STRING)
        {
            lexer->currToken = TOKEN_TYPE_STRING;
            return TOKEN_TYPE_STRING;
        }
    }

    //> 现在检查单字符属性字
    if (strlen(lexer->ptrCurrLexeme) == 1)
    {
        switch (lexer->ptrCurrLexeme[0])
        {
            //> 双引号
            case '"':
                switch (lexer->iCurrLexState)
                {
                    //> 如果不是在分析字符串, 告诉词法分析器, 现在正处于字符串中
                    case LEX_STATE_NO_STRING:
                        lexer->iCurrLexState = LEX_STATE_IN_STRING;
                        break;

                    //> 如果处于字符串中, 告诉词法分析器字符串结束
                    case LEX_STATE_IN_STRING:
                        lexer->iCurrLexState = LEX_STATE_END_STRING;
                        break;
                }
                lexer->currToken = TOKEN_TYPE_QUOTE;
                break;

            //> 逗号
            case ',':
                lexer->currToken = TOKEN_TYPE_COMMA;
                break;

            //> 分号
            case ':':
                lexer->currToken = TOKEN_TYPE_COLON;
                break;

            //> 左中括号
            case '[':
                lexer->currToken = TOKEN_TYPE_OPEN_BRACKET;
                break;

            //> 右中括号
            case ']':
                lexer->currToken = TOKEN_TYPE_CLOSE_BRACKET;
                break;

            //> 左大括号
            case '{':
                lexer->currToken = TOKEN_TYPE_OPEN_BRACE;
                break;

            //> 右大括号
            case '}':
                lexer->currToken = TOKEN_TYPE_CLOSE_BRACE;
                break;

            //> 换行
            case '\n':
                lexer->currToken = TOKEN_TYPE_NEWLINE;
                break;
        }
    }

    //> 现在检查多字符属性字
    if (isStringInt(lexer->ptrCurrLexeme))
        lexer->currToken = TOKEN_TYPE_INT;

    //> 是否浮点型
    if (isStringFloat(lexer->ptrCurrLexeme))
        lexer->currToken = TOKEN_TYPE_FLOAT;

    //> 是否是标识符(也许是行标签或指令)
    if (isStringIdent(lexer->ptrCurrLexeme))
    {
        lexer->currToken = TOKEN_TYPE_IDENT;
    }

    //> 检查属性字或者_retVal

    //> 是否是setStackSize
    if (!strcmp(lexer->ptrCurrLexeme, "SETSTACKSIZE"))
        lexer->currToken = TOKEN_TYPE_SETSTACKSIZE;

    //> 是否是var/var[]
    if (!strcmp(lexer->ptrCurrLexeme, "VAR"))
        lexer->currToken = TOKEN_TYPE_VAR;

    //> 是否是func
    if (!strcmp(lexer->ptrCurrLexeme, "FUNC"))
        lexer->currToken = TOKEN_TYPE_FUNC;

    //> 是否是param
    if (!strcmp(lexer->ptrCurrLexeme, "PARAM"))
        lexer->currToken = TOKEN_TYPE_PARAM;

    //> 是否是_retVal
    if (!strcmp(lexer->ptrCurrLexeme, "_RETVAL"))
        lexer->currToken = TOKEN_TYPE_REG_RETVAL;

    //> 是否是指令
    if (getInstrByMnemonic(GLOBAL->instrTable, lexer->ptrCurrLexeme))
        lexer->currToken = TOKEN_TYPE_INSTR;

    return lexer->currToken;
}

void
/**
 * 清除注释 以\n\0结尾
 * @param ptrSourceLine
 */
stripComments(char *ptrSourceLine)
{
    uint iCurrCharIndex;
    size_t len;
    int iInstring;
    iInstring = 0;
    len       = strlen(ptrSourceLine);

    for (iCurrCharIndex = 0; iCurrCharIndex < len; iCurrCharIndex++)
    {
        if (ptrSourceLine[iCurrCharIndex] == '"')
            iInstring = iInstring ^ 1;

        if (ptrSourceLine[iCurrCharIndex] == COMMENTIDENT && !iInstring)
        {
            ptrSourceLine[iCurrCharIndex] = '\n';
            ptrSourceLine[iCurrCharIndex+1] = '\0';
            break;
        }
    }
}

void
/**
 * 去除字符串左右空白字符, 以\n\0结尾
 * @param ptrString
 */
trimWhitespace(char *ptrString)
{
    size_t iPadLength,
           iCurrCharIndex,
           iStringLength;
    iStringLength = strlen(ptrString);

    if (iStringLength > 1)
    {
        //左边空白检测
        for (iCurrCharIndex = 0; iCurrCharIndex < iStringLength; iCurrCharIndex++)
            if (!isCharWhitespace(ptrString[iCurrCharIndex]))
                break;

        iPadLength = iCurrCharIndex;
        if (iPadLength)
        {
            for (iCurrCharIndex = iPadLength; iCurrCharIndex < iStringLength; iCurrCharIndex++)
                ptrString[iCurrCharIndex - iPadLength] = ptrString[iCurrCharIndex];

            //> 置为空白让右边能检测
            for (iCurrCharIndex = iStringLength - iPadLength; iCurrCharIndex < iStringLength; iCurrCharIndex++)
                ptrString[iCurrCharIndex] = ' ';
        }

        //右边空白检测
        for (iCurrCharIndex = iStringLength-1; iCurrCharIndex > 0; iCurrCharIndex--)
            if (!isCharWhitespace(ptrString[iCurrCharIndex]))
            {
                if (ptrString[iCurrCharIndex] != '\n')
                    ptrString[++iCurrCharIndex] = '\n';
                ptrString[iCurrCharIndex+1] = '\0';
                break;
            }
    }
}