#ifndef TINYJS_PARSER_H
#define TINYJS_PARSER_H

#include "../common/JASM.h"

void initState(state *info);
state *assmblSource(global *GLOBAL);
void parser(global *GLOBAL, state *info, lexer *lexer);

#endif