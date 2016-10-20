#ifndef _STRUTIL_
#define _STRUTIL_

#include <stdio.h>
#include <string.h>

#define INF 0xFF

void addChar(char *str, char c);
void intToString(char *buffer, int number);
void floatToString(char *buffer, float number);

#endif
