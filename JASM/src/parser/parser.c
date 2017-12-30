#include "parser.h"

void
/**
 * 初始化state
 * @param info
 */
initState(state *info)
{
    info->iCurrFuncIndex = info->iInstrStreamSize =
    info->iCurrInstrIndex = info->iCurrFuncParamCount =
    info->iCurrFuncLocalDataSize = 0;
    info->iIsFuncActive = false;
    info->iIsSetStackSizeFound = false;

    info->ptrCurrFuncName = NULL;
    info->pCurrFunc = NULL;
    info->lexer.ptrCurrLexeme = NULL;
}

state
/**
 * 解析汇编源码
 * @param GLOBAL
 * @return
 */
*assmblSource(global *GLOBAL)
{
    //-------------------------
    //   第一次词法分析         #
    //-------------------------
    state *info = (state *) malloc(sizeof(state));
    initState(info);
    //> 设置词法分析器
    lexer *lexer = &info->lexer;
    resetLexer(lexer);

    //> 属性字匹配
    tokenMatch(lexer, GLOBAL, info);
    //-------------------------
    //   第二次语法分析         #
    //-------------------------
    //> 指令流分配并初始化操作列表
    GLOBAL->pInstrStream = (instr *) malloc(info->iInstrStreamSize * sizeof(instr));
    for ( int iCurrInstrIndex = 0; iCurrInstrIndex < info->iInstrStreamSize; ++iCurrInstrIndex )
        GLOBAL->pInstrStream[iCurrInstrIndex].pOpList = NULL;
    //> 重置状态
    resetLexer(lexer);
    parser(GLOBAL, info, lexer);
    printf("222\n");
    return info;
}

void
/**
 * 语法解析
 * @param GLOBAL
 * @param info
 * @param lexer
 */
parser(global *GLOBAL, state *info, lexer *lexer)
{
    while (true)
    {
        if (getNextToken (lexer, GLOBAL) == END_OF_TOKEN_STREAM )
            break;
        printf("%s\n", lexer->ptrCurrLexeme);
        switch (lexer->currToken)
        {
            //$ 函数
            case TOKEN_TYPE_FUNC:
            {
                //> 函数名字
                getNextToken(lexer, GLOBAL);
                info->pCurrFunc = getFuncByName(GLOBAL->funcTable, lexer->ptrCurrLexeme);
                info->iIsFuncActive = true;
                info->iCurrFuncParamCount = 0;
                info->iCurrFuncIndex = info->pCurrFunc->iIndex;
                while (getNextToken(lexer, GLOBAL) == TOKEN_TYPE_NEWLINE);
                break;
            }

            //$ 函数右大括号
            case TOKEN_TYPE_CLOSE_BRACE:
            {
                //> 关闭函数局部状态
                info->iIsFuncActive = false;

                //> 主函数设置
                if (!strcasecmp ( info->pCurrFunc->ptrName, MAIN_FUNC_NAME ))
                {
                    //> 主函数特定操作码值
                    GLOBAL->pInstrStream[info->iCurrInstrIndex].iOpcode = INSTR_EXIT;

                    //> 设置操作数数量
                    GLOBAL->pInstrStream [info->iCurrInstrIndex].iOpcount = 1;

                    //> 设置操作数列表
                    GLOBAL->pInstrStream[info->iCurrInstrIndex].pOpList = (op *) malloc (sizeof (op));
                    GLOBAL->pInstrStream[info->iCurrInstrIndex].pOpList[0].iType = OP_TYPE_INT;
                    GLOBAL->pInstrStream[info->iCurrInstrIndex].pOpList[0].iIntLiteral = 0;
                }
                else
                {
                    GLOBAL->pInstrStream [info->iCurrInstrIndex].iOpcode = INSTR_RET;
                    GLOBAL->pInstrStream [info->iCurrInstrIndex].iOpcount = 0;
                    GLOBAL->pInstrStream [info->iCurrInstrIndex].pOpList = NULL;
                }
                ++info->iCurrInstrIndex;
                break;
            }

            //$ param
            case TOKEN_TYPE_PARAM:
            {
                //> 标识符
                if ( getNextToken(lexer, GLOBAL) != TOKEN_TYPE_IDENT )
                    err_sys(ERROR_MSSG_IDENT_EXPECTED);

                char *ptrIdent = getCurrLexeme(lexer);

                //> 椎栈位置
                int iStackIndex = -(info->pCurrFunc->iLocalDataSize + 2 + (info->iCurrFuncParamCount + 1));

                //> 加入符号表
                if (addSymbol(GLOBAL->symbolTable, ptrIdent, 1, iStackIndex, info->iCurrFuncIndex ) == -1 )
                    err_sys(ERROR_MSSG_IDENT_REDEFINITION);

                //> 函数参数统计
                ++ info->iCurrFuncParamCount;
                break;
            }

            //$ 解析指令
            case TOKEN_TYPE_INSTR:
            {
                info->currInstr = *getInstrByMnemonic(GLOBAL->instrTable, lexer->ptrCurrLexeme);
                //> 复制相关指令操作信息
                GLOBAL->pInstrStream[info->iCurrInstrIndex].iOpcode = info->currInstr.iOpcode;
                GLOBAL->pInstrStream[info->iCurrInstrIndex].iOpcount = info->currInstr.iOpCount;
                op * pOpList = (op *) malloc (info->currInstr.iOpCount * sizeof (op) );

                //> 指令相关操作数处理
                for (int iCurrOpIndex = 0; iCurrOpIndex < info->currInstr.iOpCount; ++iCurrOpIndex)
                {
                    opTypes currOpTypes = info->currInstr.opList[iCurrOpIndex];
                    token initOpToken = getNextToken(lexer, GLOBAL);

                    //> 检测当前指令的操作数类型是否正确
                    switch (initOpToken) {
                        //$ 操作数为整数字面值
                        case TOKEN_TYPE_INT:
                            if (currOpTypes & OP_FLAG_TYPE_INT)
                            {
                                pOpList[iCurrOpIndex].iType = OP_TYPE_INT;
                                pOpList[iCurrOpIndex].iIntLiteral = atoi (lexer->ptrCurrLexeme);
                            }
                            else
                                err_sys(ERROR_MSSG_INVALID_OP);

                            break;

                        //$ 浮点型字面值
                        case TOKEN_TYPE_FLOAT:
                            if (currOpTypes & OP_FLAG_TYPE_FLOAT)
                            {
                                pOpList [ iCurrOpIndex ].iType = OP_TYPE_FLOAT;
                                pOpList [ iCurrOpIndex ].fFloatLiteral = ( float ) atof (lexer->ptrCurrLexeme);
                            }
                            else
                                err_sys(ERROR_MSSG_INVALID_OP);
                            break;

                        //$ 字符串
                        case TOKEN_TYPE_QUOTE:
                        {
                            //> 当前指令操作是否支持字符串
                            if (currOpTypes & OP_FLAG_TYPE_STRING)
                            {
                                getNextToken(lexer, GLOBAL);

                                switch (lexer->currToken)
                                {
                                    case TOKEN_TYPE_QUOTE:
                                    {
                                        pOpList [ iCurrOpIndex ].iType = OP_TYPE_INT;
                                        pOpList [ iCurrOpIndex ].iIntLiteral = 0;
                                        break;
                                    }

                                    //> 普通字符串: 为了维护文件输出结构, 加入字符串表, 用字符表相关索引来取代
                                    case TOKEN_TYPE_STRING:
                                    {
                                        char * ptrString = getCurrLexeme(lexer);

                                        int iStringIndex = addString(GLOBAL->stringTable, ptrString);

                                        if (getNextToken(lexer, GLOBAL) != TOKEN_TYPE_QUOTE )
                                            exitOnCharExpectedError('\\');

                                        pOpList[iCurrOpIndex].iType = OP_TYPE_STRING_INDEX;
                                        pOpList[iCurrOpIndex].iStringTableIndex = iStringIndex;
                                        break;
                                    }

                                    //$ 默认报错
                                    default:
                                        err_sys(ERROR_MSSG_INVALID_STRING);
                                }
                            }
                            else
                                err_sys(ERROR_MSSG_INVALID_OP);

                            break;
                        }

                        //$ 返回值寄存器
                        case TOKEN_TYPE_REG_RETVAL:
                            //> 是否支持寄存器
                            if ( currOpTypes & OP_FLAG_TYPE_REG )
                            {
                                pOpList [ iCurrOpIndex ].iType = OP_TYPE_REG;
                                pOpList [ iCurrOpIndex ].iReg = 0;
                            }
                            else
                                err_sys ( ERROR_MSSG_INVALID_OP );

                            break;

                        // Identifiers

                        //标识符
                        //-变量
                        //- 行标签
                        //- 函数名
                        //- hostAPI
                        case TOKEN_TYPE_IDENT:
                        {
                            //> 是否为变量
                            if (currOpTypes & OP_FLAG_TYPE_MEM_REF)
                            {
                                char *ptrIdent = getCurrLexeme(lexer);
                                if (!getSymbolByIdent(GLOBAL->symbolTable, ptrIdent, info->iCurrFuncIndex))
                                    err_sys(ERROR_MSSG_UNDEFINED_IDENT);

                                int iBaseIndex = getStackIndexByIdent(GLOBAL->symbolTable, ptrIdent, info->iCurrFuncIndex);

                                //> 普通变量: 椎栈绝对索引
                                //> 数组变量: 椎栈相对和相对索引 @注: 下标[1]索引为绝对索引, 因为可计算. 变量索引[VAR]只能相对
                                if (getLookAheadChar (lexer, GLOBAL) != '[' )
                                {
                                    if (getSizeByIdent (GLOBAL->symbolTable, ptrIdent, info->iCurrFuncIndex) > 1 )
                                        err_sys(ERROR_MSSG_INVALID_ARRAY_NOT_INDEXED);

                                    pOpList[iCurrOpIndex].iType = OP_TYPE_ABS_STACK_INDEX;
                                    pOpList[iCurrOpIndex].iIntLiteral = iBaseIndex;
                                }
                                else
                                {
                                    if (getSizeByIdent (GLOBAL->symbolTable, ptrIdent, info->iCurrFuncIndex) == 1 )
                                        err_sys( ERROR_MSSG_INVALID_ARRAY );

                                    if (getNextToken (lexer, GLOBAL) != TOKEN_TYPE_OPEN_BRACKET)
                                        exitOnCharExpectedError('[');

                                    token indexToken = getNextToken(lexer, GLOBAL);
                                    //> 下标索引
                                    if (indexToken == TOKEN_TYPE_INT)
                                    {

                                        int iOffsetIndex = atoi( getCurrLexeme(lexer));

                                        pOpList [ iCurrOpIndex ].iType = OP_TYPE_ABS_STACK_INDEX;
                                        pOpList [ iCurrOpIndex ].iStackIndex = iBaseIndex + iOffsetIndex;
                                    }
                                    //> 变量索引
                                    else if (indexToken == TOKEN_TYPE_IDENT)
                                    {
                                        char *ptrIndexIdent = getCurrLexeme(lexer);

                                        if (!getSymbolByIdent(GLOBAL->symbolTable, ptrIdent, info->iCurrFuncIndex))
                                            err_sys(ERROR_MSSG_UNDEFINED_IDENT);

                                        if (getSizeByIdent(GLOBAL->symbolTable, ptrIndexIdent, info->iCurrFuncIndex) > 1)
                                            err_sys(ERROR_MSSG_INVALID_ARRAY_INDEX);
                                        //> 将变量的椎栈索引用于偏移
                                        int iOffsetIndex = getStackIndexByIdent(GLOBAL->symbolTable, ptrIdent, info->iCurrFuncIndex );

                                        pOpList[iCurrOpIndex].iType = OP_TYPE_REL_STACK_INDEX;
                                        pOpList[iCurrOpIndex].iStackIndex = iBaseIndex;
                                        pOpList[iCurrOpIndex].iOffsetIndex = iOffsetIndex;
                                    }
                                    else
                                    {
                                        err_sys(ERROR_MSSG_INVALID_ARRAY_INDEX);
                                    }

                                    if (getNextToken(lexer, GLOBAL) != TOKEN_TYPE_CLOSE_BRACKET)
                                        exitOnCharExpectedError(']');
                                }
                            }

                            //> 行标签
                            if (currOpTypes & OP_FLAG_TYPE_LINE_LABEL)
                            {
                                char *ptrLabelIdent = getCurrLexeme(lexer);

                                labelNode *pLabel = getLabelByIdent (GLOBAL->labelTable, ptrLabelIdent, info->iCurrFuncIndex );

                                if (!pLabel)
                                    err_sys(ERROR_MSSG_UNDEFINED_LINE_LABEL);

                                pOpList[iCurrOpIndex].iType = OP_TYPE_INSTR_INDEX;
                                pOpList[iCurrOpIndex].iInstrIndex = pLabel->iTargetIndex;
                            }

                            //> 函数
                            if (currOpTypes & OP_FLAG_TYPE_FUNC_NAME)
                            {
                                char *ptrFuncName = getCurrLexeme(lexer);

                                funcNode *pFunc = getFuncByName (GLOBAL->funcTable, ptrFuncName);

                                if ( ! pFunc )
                                    err_sys(ERROR_MSSG_UNDEFINED_FUNC);

                                pOpList[iCurrOpIndex].iType = OP_TYPE_FUNC_INDEX;
                                pOpList[iCurrOpIndex].iFuncIndex = pFunc->iIndex;
                            }

                            //> hostAPI
                            if (currOpTypes & OP_FLAG_TYPE_HOST_API_CALL)
                            {
                                char *ptrHostAPICall = getCurrLexeme(lexer);

                                int iIndex = addString(GLOBAL->hostAPICallTable, ptrHostAPICall);

                                pOpList [ iCurrOpIndex ].iType = OP_TYPE_HOST_API_CALL_INDEX;
                                pOpList [ iCurrOpIndex ].iHostAPICallIndex = iIndex;
                            }
                            break;
                        }
                        default:
                            err_sys(ERROR_MSSG_INVALID_OP);
                            break;
                    }
                    //> 如果还有操作数, 忽略逗号分隔符
                    if ( iCurrOpIndex < info->currInstr.iOpCount-1 )
                        if (getNextToken(lexer, GLOBAL) != TOKEN_TYPE_COMMA )
                            exitOnCharExpectedError(',');
                }
            }
        }
    }
}