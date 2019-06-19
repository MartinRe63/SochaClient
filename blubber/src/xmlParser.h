#ifndef __XMP_PARSER_H__
#define __XMP_PARSER_H__

#include "types.h"
int parseMassage(int sockFD);
void parseTurn(LastMove m, char buffer[1024]);

#endif