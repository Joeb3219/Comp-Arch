#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Gets a file if it exists, in read-only mode.
FILE* getFile(char *fileName){
	if(fileName == 0) return 0;
	return fopen(fileName, "r");
}

// Gets a file if it exists, or creates it otherwise, in read-write mode.
FILE* getFileOrCreate(char *fileName){
	if(fileName == 0) return 0;
	return fopen(fileName, "w+");
}

// Closes a file & frees memory.
int closeFile(FILE *file){
	if(file == 0) return 0;
	return fclose(file);
}

// Appends a character to a string.
void append(char *str, char c){
	char buffer[2];
	buffer[0] = c;
	buffer[1] = '\0';
	strcat(str, buffer);
}

// Searches through a file and returns the next token.
// Searching is done in place, via the file pointer.
// Thus, resetting the file pointer will begin the search for tokens at the top.
// We support inline comments denoted by a #. If a line has a hashtag and it's not within a quotationmark, we will skip to the end of the line.
// We also support quotation marks for strings, and commas for use in y86 mnemonic instructions.
char* getNextToken(FILE *file){
	char* token = malloc(8), currentChar;		// The current token is initilized to 8.
	token[0] = '\0';
	int tokens = 0, quotationEncountered = 0;					// QuotationEncountered flag will tell us not to return a token due to whitespace if within a quotation mark.
	while( (currentChar = fgetc(file)) != EOF){					// Go until EOF
		if(currentChar == '\0') return token;					// If we reach a new line, return the token.
		if(currentChar == '"') quotationEncountered = !quotationEncountered;	// If we see a quotation mark, toggle the flag.
		if(currentChar == '#' && quotationEncountered == 0){			// If we haven't encountered a quotation mark and we see a # (comment), move to the end of the line.
			// This entire structure allows inline comments.
			while(currentChar != '\n' && currentChar != EOF && currentChar != '\0') currentChar = fgetc(file);
			if(tokens > 0) return token;
			continue;
		}
		// We allow commas for use in y86 mneumonics.
		if(((currentChar <= ' ' || currentChar == ',') && quotationEncountered == 0) && tokens > 0) return token;
		if((currentChar <= ' ' || currentChar == ',') && quotationEncountered == 0) continue;
		else if(currentChar != '"'){
			if(tokens > 0 && (tokens + 1) % 8 == 0) token = realloc(token, tokens + 8);
			append(token, currentChar);
			tokens ++;
		}
	}
	return token;
}

// Writes to console and prints to file at the same time.
// A function to save two lines of code (bless, "optimizations").
void writeAndPrint(char *str, FILE *output){
	printf("%s\n", str);
	fprintf(output, "%s\n", str);
}
