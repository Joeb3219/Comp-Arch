#ifndef FUTIL_H_
#define FTUIL_H

FILE* getFile(char *filename);
FILE* getFileOrCreate(char *filename);
int closeFile(FILE *file);
char* append(char *str, char c);
char* getNextToken(FILE *file);
char* strrev(char *str);

#endif
