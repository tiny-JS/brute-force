#ifndef TINYJS_INIT_H
#define TINYJS_INIT_H

#include "../common/JASM.h"

void init(int argc, char *argv[], global *GLOBAL);
void initTable(global *GLOBAL);
void initGLOBAL(global *GLOBAL);
void initIO(int argc, char *argv[], global *GLOBAL);
void initInstrTable(linkedList *instrTable);
void loadSource(global *GLOBAL);
void buildJSE(global *GLOBAL, state *info);
#endif