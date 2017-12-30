#ifndef TINYJS_JVM_H
#define TINYJS_JVM_H
typedef struct
{
    int iGlobalDataSize;

    int iIsMainFuncPresent;
    int iMainFuncIndex;
    int iIsPaused;
    int iPauseEndTime;

    value _retVal;

    instrStream instrStream;
    runtimeStack stack;
    func *pFuncTable;
    hostAPICallTable hostAPICallTable;
} script;
#endif
