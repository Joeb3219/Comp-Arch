#ifndef _STRUTIL_
#define _STRUTIL_

#include <stdlib.h>
#include <string.h>
#define INF 0xFF

typedef unsigned char uchar;

void addChar(char *str, char c);
void intToString(char *buffer, int number);
void floatToString(char *buffer, float number);
char digToChar(int i);
int charToDig(char c);
uchar* copyArray(uchar *arr, int size);

#endif
