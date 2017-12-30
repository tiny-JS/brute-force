#include "common/JASM.h"

void
main(int argc, char *argv[])
{
    global GLOBAL;
    state *info;

    init(argc, argv, &GLOBAL);
    printf("Assembling... %s \n\n", GLOBAL.ptrSourceFileName);
    info = assmblSource(&GLOBAL);
    buildJSE(&GLOBAL, info);
    printAssmblStats(&GLOBAL, info);
}