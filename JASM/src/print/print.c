#include "print.h"

void
/**
 * 编译器信息
 * @param GLOBAL
 */
printLogo(global *GLOBAL)
{
    printf ("JASM\n");
    printf ("tinyJS Assembler Version %d.%d\n", GLOBAL->VERSION_MAJOR, GLOBAL->VERSION_MINOR);
    printf ("\n");
}

void
/**
 * 编译器选项提示
 */
printUsage()
{
    printf ( "Usage:\tJASM Source.JASM [Executable.JSE]\n" );
    printf ( "\n" );
    printf ( "\t- File extensions are not required.\n" );
    printf ( "\t- Executable name is optional; source name is used by default.\n" );
}

void
printAssmblStats(global *GLOBAL, state *info)
{
    int iVarCount = 0,
            iArrayCount = 0,
            iGlobalCount = 0;

    linkedListNode * pCurrNode = GLOBAL->symbolTable->pHead;

    for (int iCurrNode = 0; iCurrNode < GLOBAL->symbolTable->iNodeCount; ++iCurrNode)
    {
        symbolNode * pCurrSymbol = (symbolNode *) pCurrNode->pData;

        if (pCurrSymbol->iSize > 1)
            ++iArrayCount;
        else
            ++iVarCount;

        if (pCurrSymbol->iStackIndex >= 0)
            ++iGlobalCount;

        pCurrNode = pCurrNode->pNext;
    }

    printf ("%s created successfully!\n\n", GLOBAL->ptrOutputFileName);
    printf ("Source Lines Processed: %d\n", (int) GLOBAL->iSourceCodeLine);

    printf ("            Stack Size: ");
    if (GLOBAL->headerInfo.iStackSize)
        printf ("%d", GLOBAL->headerInfo.iStackSize);
    else
        printf ("Default");

    printf ("\n");
    printf ("Instructions Assembled: %d\n", info->iInstrStreamSize);
    printf ("             Variables: %d\n", iVarCount);
    printf ("                Arrays: %d\n", iArrayCount);
    printf ("               Globals: %d\n", iGlobalCount);
    printf ("       String Literals: %d\n", GLOBAL->stringTable->iNodeCount);
    printf ("                Labels: %d\n", GLOBAL->labelTable->iNodeCount);
    printf ("        Host API Calls: %d\n", GLOBAL->hostAPICallTable->iNodeCount);
    printf ("             Functions: %d\n", GLOBAL->funcTable->iNodeCount);

    printf ("      _Main () Present: ");
    if (GLOBAL->headerInfo.iIsMainFuncPresent)
        printf ("Yes (Index %d)\n", (int) GLOBAL->headerInfo.iMainFuncIndex);
    else
        printf ("No\n");
}