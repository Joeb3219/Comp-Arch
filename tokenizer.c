/*
 * Written by Joseph A. Boyle
 * NetID: jab835
 * Email: joseph.a.boyle@rutgers.edu
 * tokenizer.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Token_type enum
 * Used to differentiate the types of tokens
 *
 */
typedef enum {DECIMAL, OCTAL, HEXADECIMAL, FLOAT, ERROR, MALFORMED} Token_Type;

/*
 * Tokenizer type.  You need to fill in the type as part of your implementation.
 */

struct TokenizerT_ {
	char *original;
	int currentPos;
};

typedef struct TokenizerT_ TokenizerT;

/*
 * TKCreate creates a new TokenizerT object for a given token stream
 * (given as a string).
 * 
 * TKCreate should copy the arguments so that it is not dependent on
 * them staying immutable after returning.  (In the future, this may change
 * to increase efficiency.)
 *
 * If the function succeeds, it returns a non-NULL TokenizerT.
 * Else it returns NULL.
 *
 * You need to fill in this function as part of your implementation.
 */

TokenizerT *TKCreate( char * ts ) {
	if(ts == 0) return NULL;
	TokenizerT *token = malloc(sizeof(char *));//{ts};
	token->original = strdup(ts);
	token->currentPos = 0;
	return token;
}

/*
 * TKDestroy destroys a TokenizerT object.  It should free all dynamically
 * allocated memory that is part of the object being destroyed.
 *
 * You need to fill in this function as part of your implementation.
 */

void TKDestroy( TokenizerT * tk ) {
	free(tk);
}

/*
 * TKGetNextToken returns the next token from the token stream as a
 * character string.  Space for the returned token should be dynamically
 * allocated.  The caller is responsible for freeing the space once it is
 * no longer needed.
 *
 * If the function succeeds, it returns a C string (delimited by '\0')
 * containing the token.  Else it returns 0.
 *
 * You need to fill in this function as part of your implementation.
 */

char *TKGetNextToken( TokenizerT * tk ) {
	int i = tk->currentPos, beginningIndex = -1;
	char c = 0, *token;

	for(i = tk->currentPos; i < strlen(tk->original); i ++){
		c = tk->original[i];
		if(c == 0x20 || (c >= 0x09 && c <= 0x0d)){
			if(beginningIndex == -1) continue;
			break;
		}else if(beginningIndex == -1) beginningIndex = i;
	}

	if(beginningIndex == -1 || i - beginningIndex <= 0) return 0; //There are no more tokens left!
	tk->currentPos = i;
	
	token = (char*) malloc(sizeof(char) * (i - beginningIndex + 1));
	strncpy(token, &tk->original[beginningIndex], (i - beginningIndex));
	return token;

}

Token_Type TKIdentifyToken( char *token){
	int i = 0, j = 0, numDecimals = 0;
	char *charPtr;

	if(strlen(token) == 0) return ERROR; // A zero length string, which is an error.
	
	// Check if the string contains e/E
	if( (charPtr = strchr(token, 'E')) || (charPtr = strchr(token, 'e')) ){
		j = charPtr - token;
		if(i == j || j + 2 >= strlen(token)) return MALFORMED; //If e is the first character, then we have a malformed input.
		for(i = 0; i < j; i ++){
			if(token[i] == '.') numDecimals ++;
			else if(!(token[i] >= '0' && token[i] <= '9')) return MALFORMED; // If there are non decimal, non numeric values, then this isn't a float.
		}
		if(numDecimals != 1) return MALFORMED; // This isn't a float as it contains a number of decimals that isn't one.
		if(token[i + 1] != '+' && token[i + 1] != '-') return MALFORMED; // If there isn't a +/-, this isnt a float.
		i = j + 2;
		j = strlen(token);
		for(i; i < j; i ++){
			if(!(token[i] >= '0' && token[i] <= '9')) return MALFORMED;
		}
		return FLOAT; // All checks pass, this is a float.
	}
	// Indicative of the string being either 
	if(token[0] == '0'){
		
	}
	return DECIMAL;
}

const char* getTokenTypeName(Token_Type type){
	switch(type){
		case FLOAT: return "Float";
		case DECIMAL: return "Decimal";
		case OCTAL: return "Octal";
		case HEXADECIMAL: return "Hexadecimal";
		case MALFORMED: return "Malformed";
		default: return "[Err]";
	}
}

/*
 * main will have a string argument (in argv[1]).
 * The string argument contains the tokens.
 * Print out the tokens in the second string in left-to-right order.
 * Each token should be printed on a separate line.
 */

int main(int argc, char **argv) {
	char *currentToken;
	TokenizerT *tokenizer;
 	
	tokenizer= TKCreate(argv[1]);
	printf("Tokenizer: %s\n", tokenizer->original);
	
	while((currentToken = TKGetNextToken(tokenizer)) != 0){
		printf("%s: %s\n", getTokenTypeName(TKIdentifyToken(currentToken)), currentToken);
	}
	
	TKDestroy(tokenizer);

	return 0;
}

