#include <stdlib.h>
#include <stdio.h>
#include <string.h>

FILE* getFile(char *fileName){
	if(fileName == 0) return 0;
	return fopen(fileName, "r");
}

FILE* getFileOrCreate(char *fileName){
	if(fileName == 0) return 0;
	return fopen(fileName, "w+");
}

int closeFile(FILE *file){
	if(file == 0) return 0;
	return fclose(file);
}

void append(char *str, char c){
	char buffer[2];
	buffer[0] = c;
	buffer[1] = '\0';
	strcat(str, buffer);
}

char* getNextToken(FILE *file){
	char* token = malloc(8), currentChar;
	token[0] = '\0';
	int tokens = 0, quotationEncountered = 0;
	while( (currentChar = fgetc(file)) != EOF){
		if(currentChar == '"') quotationEncountered = !quotationEncountered;
		if((currentChar <= ' ' && quotationEncountered == 0) && tokens > 0) return token;
		else if(currentChar != '"'){
			if(tokens > 0 && (tokens + 1) % 8 == 0) token = realloc(token, tokens + 8);
			append(token, currentChar);
			tokens ++;
		}
	}
	return token;
}
