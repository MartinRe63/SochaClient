#ifndef __XMP_PARSER_H__
#define __XMP_PARSER_H__

#include "SuperPackedMove.h"
int parseMassage(int sockFD);
void parseTurn(LastMove m, char buffer[1024]);

#endif