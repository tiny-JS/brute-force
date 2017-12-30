#include "init.h"

void
/**
 * 初始化编译配置
 * @param argc
 * @param argv
 * @param GLOBAL
 */
init(int argc, char *argv[], global *GLOBAL)
{
    //$ 全局数据初始化
    initGLOBAL(GLOBAL);

    //> 初始化所有表
    initTable(GLOBAL);

    //> 初始化IO文件名
    initIO(argc, argv, GLOBAL);

    //> 加载源文件数据
    loadSource(GLOBAL);
}


void
/**
 * 初始化输入输出文件
 * @param argc
 * @param argv
 * @param GLOBAL
 */
initIO(int argc, char *argv[], global *GLOBAL)
{
    //> 检查命令行参数
    if (argc < 2)
    {
        printUsage();
        exit(1);
    }

    //> 设置输入文件名
    GLOBAL->ptrSourceFileName = argv[1];

    //> 设置输出文件名
    if (argv[2])
    {
        GLOBAL->ptrOutputFileName = argv[2];
    }
    else
    {
        size_t len = strlen(GLOBAL->ptrSourceFileName);
        char *pTmp = (char *) malloc(len);
        strcpy(pTmp, GLOBAL->ptrSourceFileName);
        strtok(pTmp, ".");
        len = strlen(pTmp);
        GLOBAL->ptrOutputFileName = (char *) malloc(len+4);
        strcpy(GLOBAL->ptrOutputFileName, pTmp);
        free(pTmp);

        strcat(GLOBAL->ptrOutputFileName, ".JSE");
    }
}

void
/**
 * 初始化全局属性 @<无问题>
 * @param GLOBAL
 */
initGLOBAL(global *GLOBAL)
{
    GLOBAL->iSourceCodeLine = 0;
    //> 初始化编译器版本信息
    GLOBAL->VERSION_MAJOR = 0;
    GLOBAL->VERSION_MINOR = 1;

    //> 初始化头部信息
    GLOBAL->headerInfo.iGlobalDateSize = 0;
    GLOBAL->headerInfo.iStackSize = 0;
    GLOBAL->headerInfo.iIsMainFuncPresent = false;
    GLOBAL->headerInfo.iMainFuncIndex = 0;

    //> 初始化表指针
    GLOBAL->stringTable      = (linkedList *) malloc(sizeof(linkedList));
    GLOBAL->funcTable        = (linkedList *) malloc(sizeof(linkedList));
    GLOBAL->labelTable       = (linkedList *) malloc(sizeof(linkedList));
    GLOBAL->hostAPICallTable = (linkedList *) malloc(sizeof(linkedList));
    GLOBAL->symbolTable      = (linkedList *) malloc(sizeof(linkedList));
    GLOBAL->instrTable       = (linkedList *) malloc(sizeof(linkedList));
}

void
/**
 * 初始化所有表 @<无问题>
 * @param GLOBAL
 */
initTable(global *GLOBAL)
{
    initLinkedList(GLOBAL->stringTable);
    initLinkedList(GLOBAL->funcTable);
    initLinkedList(GLOBAL->symbolTable);
    initLinkedList(GLOBAL->labelTable);
    initLinkedList(GLOBAL->hostAPICallTable);
    initLinkedList(GLOBAL->instrTable);
    initInstrTable(GLOBAL->instrTable);
}

void
buildJSE(global *GLOBAL, state *info)
{
    FILE * pExecFile;
    if (!( pExecFile = fopen ( GLOBAL->ptrOutputFileName, "w" )))
        err_sys("Could not open executable file for output");

    //$ 二进制文件头部信息
    fwrite(IDSTRING, 4, 1, pExecFile);

    //> 编译器版本信息

    int *cVersionMajor = &GLOBAL->VERSION_MAJOR,
         *cVersionMinor = &GLOBAL->VERSION_MINOR;
    fwrite (cVersionMajor, 1, 1, pExecFile);
    fwrite (cVersionMinor, 1, 1, pExecFile);

    //> 椎栈信息
    fwrite (&GLOBAL->headerInfo.iStackSize, 4, 1, pExecFile);

    //> 全局内存信息
    fwrite (&GLOBAL->headerInfo.iGlobalDateSize, 4, 1, pExecFile);

    //> 是否有main函数以及其函数位置
    char cIsMainPresent = 0;
    if ( GLOBAL->headerInfo.iIsMainFuncPresent)
        cIsMainPresent = 1;
    fwrite (&cIsMainPresent, 1, 1, pExecFile);

    fwrite (&GLOBAL->headerInfo.iMainFuncIndex, 4, 1, pExecFile);

    //$ 指令流(instruction streams)
    fwrite (&info->iInstrStreamSize, sizeof(info->iInstrStreamSize), 1, pExecFile);

    for (int iCurrInstrIndex = 0; iCurrInstrIndex < info->iInstrStreamSize; ++iCurrInstrIndex)
    {
        //> 指令相关操作码值
        short sOpcode = GLOBAL->pInstrStream[iCurrInstrIndex].iOpcode;
        fwrite (&sOpcode, sizeof(int), 1, pExecFile);

        //> 相关指令操作数
        char iOpCount = GLOBAL->pInstrStream[iCurrInstrIndex].iOpcount;
        fwrite (&iOpCount, 1, 1, pExecFile);

        for (int iCurrOpIndex = 0; iCurrOpIndex < iOpCount; ++ iCurrOpIndex)
        {
            op CurrOp = GLOBAL->pInstrStream[iCurrInstrIndex].pOpList[iCurrOpIndex];

            char cOpType = CurrOp.iType;
            fwrite (&cOpType, 1, 1, pExecFile);

            //> 操作数类型不同
            switch (CurrOp.iType)
            {
                case OP_TYPE_INT:
                    fwrite (&CurrOp.iIntLiteral, sizeof (int), 1, pExecFile);
                    break;

                case OP_TYPE_FLOAT:
                    fwrite (&CurrOp.fFloatLiteral, sizeof (float), 1, pExecFile);
                    break;

                case OP_TYPE_STRING_INDEX:
                    fwrite (&CurrOp.iStringTableIndex, sizeof (int), 1, pExecFile);
                    break;

                case OP_TYPE_INSTR_INDEX:
                    fwrite (& CurrOp.iInstrIndex, sizeof (int), 1, pExecFile);
                    break;

                    // Absolute stack index

                case OP_TYPE_ABS_STACK_INDEX:
                    fwrite (& CurrOp.iStackIndex, sizeof ( int ), 1, pExecFile);
                    break;

                    // Relative stack index

                case OP_TYPE_REL_STACK_INDEX:
                    fwrite (& CurrOp.iStackIndex, sizeof ( int ), 1, pExecFile);
                    fwrite (& CurrOp.iOffsetIndex, sizeof ( int ), 1, pExecFile);
                    break;

                    // Function index

                case OP_TYPE_FUNC_INDEX:
                    fwrite (& CurrOp.iFuncIndex, sizeof ( int ), 1, pExecFile);
                    break;

                    // Host API call index

                case OP_TYPE_HOST_API_CALL_INDEX:
                    fwrite (& CurrOp.iHostAPICallIndex, sizeof ( int ), 1, pExecFile);
                    break;

                    // Register

                case OP_TYPE_REG:
                    fwrite (& CurrOp.iReg, sizeof ( int ), 1, pExecFile);
                    break;
            }
        }
    }

    int iCurrNode;
    linkedListNode * pNode;

    //> 字符串表
    fwrite ( & GLOBAL->stringTable->iNodeCount, 4, 1, pExecFile);

    pNode = GLOBAL->stringTable->pHead;
    char cParamCount;

    for (iCurrNode = 0; iCurrNode < GLOBAL->stringTable->iNodeCount; ++ iCurrNode)
    {
        char *ptrCurrString = (char *) pNode->pData;
        int iCurrStringLength = strlen ( ptrCurrString );

        fwrite ( & iCurrStringLength, 4, 1, pExecFile );
        fwrite ( ptrCurrString, strlen ( ptrCurrString ), 1, pExecFile );

        pNode = pNode->pNext;
    }

    //> 函数表

    fwrite (&GLOBAL->funcTable->iNodeCount, 4, 1, pExecFile);

    pNode = GLOBAL->funcTable->pHead;
    for (iCurrNode = 0; iCurrNode < GLOBAL->funcTable->iNodeCount; ++ iCurrNode)
    {

        funcNode * pFunc = ( funcNode * ) pNode->pData;

        fwrite ( & pFunc->iEntryPoint, sizeof(int), 1, pExecFile);

        cParamCount = pFunc->iParamCount;
        fwrite (&cParamCount, 1, 1, pExecFile);

        //> 局部数据大小记录
        fwrite (&pFunc->iLocalDataSize, sizeof (int), 1, pExecFile);

        pNode = pNode->pNext;
    }

    //> HostAPITable

    fwrite (&GLOBAL->hostAPICallTable->iNodeCount, 4, 1, pExecFile);

    pNode = GLOBAL->hostAPICallTable->pHead;

    for (iCurrNode = 0; iCurrNode < GLOBAL->hostAPICallTable->iNodeCount; ++iCurrNode)
    {
        char * ptrCurrHostAPICall = (char *) pNode->pData;
        char cCurrHostAPICallLength = strlen(ptrCurrHostAPICall);

        fwrite ( & cCurrHostAPICallLength, 1, 1, pExecFile );
        fwrite ( ptrCurrHostAPICall, strlen ( ptrCurrHostAPICall ), 1, pExecFile );

        pNode = pNode->pNext;
    }

    fclose ( pExecFile );
}


void
/**
 * 加载源文件代码到堆栈中 @<安全>
 * @param GLOBAL
 */
loadSource(global *GLOBAL)
{
    //$ 加载源代码入缓冲区中, 每一行对应数组一行
    FILE *pSourceFile;
    char *dupLine = NULL; //当前行指针
    size_t maxLineSize = 512;
    pSourceFile = fopen(GLOBAL->ptrSourceFileName, "r");
    if (!pSourceFile)
        err_sys("fopen");

    //> 计算文件有多少行, 以便于申请缓冲区
    while (!feof(pSourceFile))
        if (fgetc(pSourceFile) == '\n')
            GLOBAL->iSourceCodeLine++;
    GLOBAL->ptrSourceCode = (char **) malloc(GLOBAL->iSourceCodeLine * sizeof(char *));

    //> 重置流位置, 且源码每行对应数组中的一行
    if (fseek(pSourceFile, 0, SEEK_SET))
        err_sys("fseek");
    for (size_t i = 0; i <= GLOBAL->iSourceCodeLine; i++)
    {
        char ptrLine[maxLineSize];
        while (fgets(ptrLine, (int) maxLineSize, pSourceFile))
        {
            char *pTmp;
            size_t ptrLineSize = strlen(ptrLine);
            //> 复制数据: dupLine不为NULL, 则在此之前有数据, 需要一起复制
            if (dupLine)
            {
                size_t dupLineSize = strlen(dupLine);
                pTmp = (char *) malloc(dupLineSize + ptrLineSize);
                if (!pTmp)
                    err_sys("malloc");
                strcpy(pTmp, dupLine);
                //> 偏移指针防止之前的数据被覆盖
                strcpy(&pTmp[0]+dupLineSize, ptrLine);
                //> 释放以前的内存
                free(dupLine);
                dupLine = pTmp;
            }
            else
            {
                dupLine = (char *) malloc(ptrLineSize);
                if (!dupLine)
                    err_sys("malloc");
                strcpy(dupLine, ptrLine);
            }
            //> 出现换行符即当前行已经读取完毕, 需要跳出while
            if (ptrLine[ptrLineSize-1] == '\n' || ptrLineSize < maxLineSize)
                break;
        }
        //> 错误检测
        if (ferror(pSourceFile))
            err_sys("fgets");
        //> 到达文件尾后如果dupLine不为NULL, 证明是最后一行, 仍然需要复制
        if (feof(pSourceFile) && dupLine == NULL)
            break;
        //> 清除注释
        stripComments(dupLine);
        //> 清除两端空白符号(不包括\n)
        trimWhitespace(dupLine);
        GLOBAL->ptrSourceCode[i] = dupLine;
        //> 重置dupLine
        dupLine = NULL;
    }
    fclose(pSourceFile);
}

void
/**
 * 指令添加
 * @param instrTable
 */
initInstrTable (linkedList *instrTable)
{
    int iInstrIndex;

    // Mov          Destination, Source
    iInstrIndex = addInstrLookup (instrTable, "mov", INSTR_MOV, 2);
    setOpType (instrTable, iInstrIndex, 0, OP_FLAG_TYPE_MEM_REF |
                                           OP_FLAG_TYPE_REG);
    setOpType (instrTable, iInstrIndex, 1,OP_FLAG_TYPE_INT     |
                                          OP_FLAG_TYPE_FLOAT   |
                                          OP_FLAG_TYPE_STRING  |
                                          OP_FLAG_TYPE_MEM_REF |
                                          OP_FLAG_TYPE_REG);

    // Add         Destination, Source
    iInstrIndex = addInstrLookup(instrTable, "add", INSTR_ADD, 2);
    setOpType (instrTable, iInstrIndex, 0, OP_FLAG_TYPE_MEM_REF|
                                           OP_FLAG_TYPE_REG );
    setOpType(instrTable, iInstrIndex, 1, OP_FLAG_TYPE_INT |
                                          OP_FLAG_TYPE_FLOAT |
                                          OP_FLAG_TYPE_STRING |
                                          OP_FLAG_TYPE_MEM_REF |
                                          OP_FLAG_TYPE_REG);

    // Sub          Destination, Source
    iInstrIndex = addInstrLookup(instrTable, "sub", INSTR_SUB, 2 );
    setOpType(instrTable, iInstrIndex, 0, OP_FLAG_TYPE_MEM_REF |
                                           OP_FLAG_TYPE_REG );
    setOpType(instrTable, iInstrIndex, 1, OP_FLAG_TYPE_INT   |
                                           OP_FLAG_TYPE_FLOAT |
                                           OP_FLAG_TYPE_STRING |
                                           OP_FLAG_TYPE_MEM_REF |
                                           OP_FLAG_TYPE_REG );

    // Mul          Destination, Source
    iInstrIndex = addInstrLookup(instrTable, "mul", INSTR_MUL, 2);
    setOpType(instrTable, iInstrIndex, 0, OP_FLAG_TYPE_MEM_REF |
                                           OP_FLAG_TYPE_REG);
    setOpType(instrTable, iInstrIndex, 1, OP_FLAG_TYPE_INT |
                                           OP_FLAG_TYPE_FLOAT |
                                           OP_FLAG_TYPE_STRING |
                                           OP_FLAG_TYPE_MEM_REF |
                                           OP_FLAG_TYPE_REG );

    // Div          Destination, Source
    iInstrIndex = addInstrLookup (instrTable, "div", INSTR_DIV, 2);
    setOpType(instrTable, iInstrIndex, 0, OP_FLAG_TYPE_MEM_REF |
                                OP_FLAG_TYPE_REG );
    setOpType(instrTable, iInstrIndex, 1, OP_FLAG_TYPE_INT |
                                OP_FLAG_TYPE_FLOAT |
                                OP_FLAG_TYPE_STRING |
                                OP_FLAG_TYPE_MEM_REF |
                                OP_FLAG_TYPE_REG );

    // Mod          Destination, Source
    iInstrIndex = addInstrLookup (instrTable, "mod", INSTR_MOD, 2 );
    setOpType(instrTable, iInstrIndex, 0, OP_FLAG_TYPE_MEM_REF |
                                OP_FLAG_TYPE_REG );
    setOpType(instrTable, iInstrIndex, 1, OP_FLAG_TYPE_INT |
                                OP_FLAG_TYPE_FLOAT |
                                OP_FLAG_TYPE_STRING |
                                OP_FLAG_TYPE_MEM_REF |
                                OP_FLAG_TYPE_REG );

    // Exp          Destination, Source
    iInstrIndex = addInstrLookup (instrTable, "exp", INSTR_EXP, 2 );
    setOpType(instrTable, iInstrIndex, 0, OP_FLAG_TYPE_MEM_REF |
                                OP_FLAG_TYPE_REG );
    setOpType(instrTable, iInstrIndex, 1, OP_FLAG_TYPE_INT |
                                OP_FLAG_TYPE_FLOAT |
                                OP_FLAG_TYPE_STRING |
                                OP_FLAG_TYPE_MEM_REF |
                                OP_FLAG_TYPE_REG );

    // Neg          Destination
    iInstrIndex = addInstrLookup (instrTable, "neg", INSTR_NEG, 1 );
    setOpType (instrTable, iInstrIndex, 0, OP_FLAG_TYPE_MEM_REF |
                                OP_FLAG_TYPE_REG );

    // Inc          Destination
    iInstrIndex = addInstrLookup (instrTable, "inc", INSTR_INC, 1 );
    setOpType (instrTable, iInstrIndex, 0, OP_FLAG_TYPE_MEM_REF |
                                OP_FLAG_TYPE_REG );

    // Dec          Destination
    iInstrIndex = addInstrLookup (instrTable, "dec", INSTR_DEC, 1 );
    setOpType (instrTable, iInstrIndex, 0, OP_FLAG_TYPE_MEM_REF |
                                OP_FLAG_TYPE_REG );


    // And          Destination, Source
    iInstrIndex = addInstrLookup (instrTable, "and", INSTR_AND, 2 );
    setOpType (instrTable, iInstrIndex, 0, OP_FLAG_TYPE_MEM_REF |
                                OP_FLAG_TYPE_REG );
    setOpType (instrTable, iInstrIndex, 1, OP_FLAG_TYPE_INT |
                                OP_FLAG_TYPE_FLOAT |
                                OP_FLAG_TYPE_STRING |
                                OP_FLAG_TYPE_MEM_REF |
                                OP_FLAG_TYPE_REG );

    // Or           Destination, Source
    iInstrIndex = addInstrLookup (instrTable, "or", INSTR_OR, 2 );
    setOpType (instrTable, iInstrIndex, 0, OP_FLAG_TYPE_MEM_REF |
                                OP_FLAG_TYPE_REG );
    setOpType (instrTable, iInstrIndex, 1, OP_FLAG_TYPE_INT |
                                OP_FLAG_TYPE_FLOAT |
                                OP_FLAG_TYPE_STRING |
                                OP_FLAG_TYPE_MEM_REF |
                                OP_FLAG_TYPE_REG );

    // XOr          Destination, Source
    iInstrIndex = addInstrLookup (instrTable, "xor", INSTR_XOR, 2 );
    setOpType (instrTable, iInstrIndex, 0, OP_FLAG_TYPE_MEM_REF |
                                OP_FLAG_TYPE_REG );
    setOpType (instrTable, iInstrIndex, 1, OP_FLAG_TYPE_INT |
                                OP_FLAG_TYPE_FLOAT |
                                OP_FLAG_TYPE_STRING |
                                OP_FLAG_TYPE_MEM_REF |
                                OP_FLAG_TYPE_REG );

    // Not          Destination
    iInstrIndex = addInstrLookup (instrTable, "not", INSTR_NOT, 1 );
    setOpType (instrTable, iInstrIndex, 0, OP_FLAG_TYPE_MEM_REF |
                                OP_FLAG_TYPE_REG );

    // ShL          Destination, Source
    iInstrIndex = addInstrLookup (instrTable, "shl", INSTR_SHL, 2 );
    setOpType (instrTable, iInstrIndex, 0, OP_FLAG_TYPE_MEM_REF |
                                OP_FLAG_TYPE_REG );
    setOpType (instrTable, iInstrIndex, 1, OP_FLAG_TYPE_INT |
                                OP_FLAG_TYPE_FLOAT |
                                OP_FLAG_TYPE_STRING |
                                OP_FLAG_TYPE_MEM_REF |
                                OP_FLAG_TYPE_REG );

    // ShR          Destination, Source
    iInstrIndex = addInstrLookup (instrTable, "shr", INSTR_SHR, 2 );
    setOpType (instrTable, iInstrIndex, 0, OP_FLAG_TYPE_MEM_REF |
                                OP_FLAG_TYPE_REG );
    setOpType (instrTable, iInstrIndex, 1, OP_FLAG_TYPE_INT |
                                OP_FLAG_TYPE_FLOAT |
                                OP_FLAG_TYPE_STRING |
                                OP_FLAG_TYPE_MEM_REF |
                                OP_FLAG_TYPE_REG );

    // Concat       String0, String1
    iInstrIndex = addInstrLookup (instrTable, "concat", INSTR_CONCAT, 2 );
    setOpType (instrTable, iInstrIndex, 0, OP_FLAG_TYPE_MEM_REF |
                                OP_FLAG_TYPE_REG );
    setOpType (instrTable, iInstrIndex, 1, OP_FLAG_TYPE_MEM_REF |
                                OP_FLAG_TYPE_REG |
                                OP_FLAG_TYPE_STRING );

    // GetChar      Destination, Source, Index
    iInstrIndex = addInstrLookup (instrTable, "getchar", INSTR_GETCHAR, 3 );
    setOpType (instrTable, iInstrIndex, 0, OP_FLAG_TYPE_MEM_REF |
                                OP_FLAG_TYPE_REG );
    setOpType (instrTable, iInstrIndex, 1, OP_FLAG_TYPE_MEM_REF |
                                OP_FLAG_TYPE_REG |
                                OP_FLAG_TYPE_STRING );
    setOpType (instrTable, iInstrIndex, 2, OP_FLAG_TYPE_MEM_REF |
                                OP_FLAG_TYPE_REG |
                                OP_FLAG_TYPE_INT );

    // SetChar      Destination, Index, Source
    iInstrIndex = addInstrLookup (instrTable, "setchar", INSTR_SETCHAR, 3 );
    setOpType (instrTable, iInstrIndex, 0, OP_FLAG_TYPE_MEM_REF |
                                OP_FLAG_TYPE_REG );
    setOpType (instrTable, iInstrIndex, 1, OP_FLAG_TYPE_MEM_REF |
                                OP_FLAG_TYPE_REG |
                                OP_FLAG_TYPE_INT );
    setOpType (instrTable, iInstrIndex, 2, OP_FLAG_TYPE_MEM_REF |
                                OP_FLAG_TYPE_REG |
                                OP_FLAG_TYPE_STRING );


    // Jmp          Label
    iInstrIndex = addInstrLookup (instrTable, "jmp", INSTR_JMP, 1 );
    setOpType (instrTable, iInstrIndex, 0, OP_FLAG_TYPE_LINE_LABEL );

    // JE           Op0, Op1, Label
    iInstrIndex = addInstrLookup (instrTable, "je", INSTR_JE, 3 );
    setOpType (instrTable, iInstrIndex, 0, OP_FLAG_TYPE_INT |
                                OP_FLAG_TYPE_FLOAT |
                                OP_FLAG_TYPE_STRING |
                                OP_FLAG_TYPE_MEM_REF |
                                OP_FLAG_TYPE_REG );
    setOpType (instrTable, iInstrIndex, 1, OP_FLAG_TYPE_INT |
                                OP_FLAG_TYPE_FLOAT |
                                OP_FLAG_TYPE_STRING |
                                OP_FLAG_TYPE_MEM_REF |
                                OP_FLAG_TYPE_REG );
    setOpType (instrTable, iInstrIndex, 2, OP_FLAG_TYPE_LINE_LABEL );

    // JNE          Op0, Op1, Label
    iInstrIndex = addInstrLookup (instrTable, "jne", INSTR_JNE, 3 );
    setOpType (instrTable, iInstrIndex, 0, OP_FLAG_TYPE_INT |
                                OP_FLAG_TYPE_FLOAT |
                                OP_FLAG_TYPE_STRING |
                                OP_FLAG_TYPE_MEM_REF |
                                OP_FLAG_TYPE_REG );
    setOpType (instrTable, iInstrIndex, 1, OP_FLAG_TYPE_INT |
                                OP_FLAG_TYPE_FLOAT |
                                OP_FLAG_TYPE_STRING |
                                OP_FLAG_TYPE_MEM_REF |
                                OP_FLAG_TYPE_REG );
    setOpType (instrTable, iInstrIndex, 2, OP_FLAG_TYPE_LINE_LABEL );

    // JG           Op0, Op1, Label
    iInstrIndex = addInstrLookup (instrTable, "jg", INSTR_JG, 3 );
    setOpType (instrTable, iInstrIndex, 0, OP_FLAG_TYPE_INT |
                                OP_FLAG_TYPE_FLOAT |
                                OP_FLAG_TYPE_STRING |
                                OP_FLAG_TYPE_MEM_REF |
                                OP_FLAG_TYPE_REG );
    setOpType (instrTable, iInstrIndex, 1, OP_FLAG_TYPE_INT |
                                OP_FLAG_TYPE_FLOAT |
                                OP_FLAG_TYPE_STRING |
                                OP_FLAG_TYPE_MEM_REF |
                                OP_FLAG_TYPE_REG );
    setOpType (instrTable, iInstrIndex, 2, OP_FLAG_TYPE_LINE_LABEL );

    // JL           Op0, Op1, Label
    iInstrIndex = addInstrLookup (instrTable, "jl", INSTR_JL, 3 );
    setOpType (instrTable, iInstrIndex, 0, OP_FLAG_TYPE_INT |
                                OP_FLAG_TYPE_FLOAT |
                                OP_FLAG_TYPE_STRING |
                                OP_FLAG_TYPE_MEM_REF |
                                OP_FLAG_TYPE_REG );
    setOpType (instrTable, iInstrIndex, 1, OP_FLAG_TYPE_INT |
                                OP_FLAG_TYPE_FLOAT |
                                OP_FLAG_TYPE_STRING |
                                OP_FLAG_TYPE_MEM_REF |
                                OP_FLAG_TYPE_REG );
    setOpType (instrTable, iInstrIndex, 2, OP_FLAG_TYPE_LINE_LABEL );

    // JGE          Op0, Op1, Label
    iInstrIndex = addInstrLookup (instrTable, "jge", INSTR_JGE, 3 );
    setOpType (instrTable, iInstrIndex, 0, OP_FLAG_TYPE_INT |
                                OP_FLAG_TYPE_FLOAT |
                                OP_FLAG_TYPE_STRING |
                                OP_FLAG_TYPE_MEM_REF |
                                OP_FLAG_TYPE_REG );
    setOpType (instrTable, iInstrIndex, 1, OP_FLAG_TYPE_INT |
                                OP_FLAG_TYPE_FLOAT |
                                OP_FLAG_TYPE_STRING |
                                OP_FLAG_TYPE_MEM_REF |
                                OP_FLAG_TYPE_REG );
    setOpType (instrTable, iInstrIndex, 2, OP_FLAG_TYPE_LINE_LABEL );

    // JLE           Op0, Op1, Label
    iInstrIndex = addInstrLookup (instrTable, "jle", INSTR_JLE, 3 );
    setOpType (instrTable, iInstrIndex, 0, OP_FLAG_TYPE_INT |
                                OP_FLAG_TYPE_FLOAT |
                                OP_FLAG_TYPE_STRING |
                                OP_FLAG_TYPE_MEM_REF |
                                OP_FLAG_TYPE_REG );
    setOpType (instrTable, iInstrIndex, 1, OP_FLAG_TYPE_INT |
                                OP_FLAG_TYPE_FLOAT |
                                OP_FLAG_TYPE_STRING |
                                OP_FLAG_TYPE_MEM_REF |
                                OP_FLAG_TYPE_REG );
    setOpType (instrTable, iInstrIndex, 2, OP_FLAG_TYPE_LINE_LABEL );

    // Push          Source
    iInstrIndex = addInstrLookup (instrTable, "push", INSTR_PUSH, 1 );
    setOpType (instrTable, iInstrIndex, 0, OP_FLAG_TYPE_INT |
                                OP_FLAG_TYPE_FLOAT |
                                OP_FLAG_TYPE_STRING |
                                OP_FLAG_TYPE_MEM_REF |
                                OP_FLAG_TYPE_REG );

    // Pop           Destination
    iInstrIndex = addInstrLookup (instrTable, "pop", INSTR_POP, 1 );
    setOpType (instrTable, iInstrIndex, 0, OP_FLAG_TYPE_MEM_REF |
                                OP_FLAG_TYPE_REG );


    // Call          FunctionName
    iInstrIndex = addInstrLookup (instrTable, "call", INSTR_CALL, 1 );
    setOpType (instrTable, iInstrIndex, 0, OP_FLAG_TYPE_FUNC_NAME );

    // Ret

    iInstrIndex = addInstrLookup (instrTable, "ret", INSTR_RET, 0 );

    // CallHost      FunctionName

    iInstrIndex = addInstrLookup (instrTable, "callhost", INSTR_CALLHOST, 1 );
    setOpType (instrTable, iInstrIndex, 0, OP_FLAG_TYPE_HOST_API_CALL );


    // Pause        Duration
    iInstrIndex = addInstrLookup (instrTable, "pause", INSTR_PAUSE, 1 );
    setOpType (instrTable, iInstrIndex, 0, OP_FLAG_TYPE_INT |
                                OP_FLAG_TYPE_FLOAT |
                                OP_FLAG_TYPE_STRING |
                                OP_FLAG_TYPE_MEM_REF |
                                OP_FLAG_TYPE_REG );

    // Exit         Code
    iInstrIndex = addInstrLookup (instrTable, "exit", INSTR_EXIT, 1 );
    setOpType (instrTable, iInstrIndex, 0, OP_FLAG_TYPE_INT |
                                OP_FLAG_TYPE_FLOAT |
                                OP_FLAG_TYPE_STRING |
                                OP_FLAG_TYPE_MEM_REF |
                                OP_FLAG_TYPE_REG );
}