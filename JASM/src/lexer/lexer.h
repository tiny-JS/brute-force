#ifndef TINYJS_LEXER_H
#define TINYJS_LEXER_H

#include "../common/JASM.h"


token getNextToken(lexer *lexer, global *GLOBAL);
void tokenMatch(lexer *lexer, global *GLOBAL, state *info);
char *getCurrLexeme(lexer *lexer);
char getLookAheadChar(lexer *lexer, global *GLOBAL);

Boolean skipToNextLine(lexer *lexer, global *conf);
void resetLexer(lexer *lexer);

//清除注释和两端空白字符
void stripComments(char *ptrSourceLine);
void trimWhitespace(char *ptrString);

#endif //TINYJS_LEXER_H
