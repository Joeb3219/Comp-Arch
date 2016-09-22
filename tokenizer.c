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
 * We use these labels to throw to various functions so that we can associate various bits of code with a defined type.
 */
typedef enum {DECIMAL, OCTAL, HEXADECIMAL, FLOAT, ZERO, ERROR, MALFORMED} Token_Type;

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
		if(c <= 0x20){
			if(!(c == 0x20 || (c >= 0x09 && c <= 0x0d))){
				printf("[%#x]\n", tk->original[i]);
			}
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

int validCharactersInRange( char *token, int beginningIndex, int endingIndex, int beginningValid, int endingValid){
	int i = 0;
	for(i = beginningIndex; i < endingIndex; i ++){
		if(!(token[i] >= beginningValid && token[i] <= endingValid)) return 0;
	}
	return 1;
}

int verifyTokenType( char *token, Token_Type type){
	int i = 0, j = 0;
	char *ePtr, *decPtr;
	switch(type){
		case OCTAL:
			if(strlen(token) < 2 || token[0] != '0') return 0;
			return validCharactersInRange(token, 0, strlen(token), '0', '7');
		case HEXADECIMAL:
			if(strlen(token) < 3 || (token[0] != '0')) return 0;
			if(token[1] != 'x' && token[1] != 'X') return 0;
			for(i = 2; i < strlen(token); i ++){
				if(!(token[i] >= '0' && token[i] <= '9') && !(token[i] >= 'a' && token[i] <= 'f') && !(token[i] >= 'A' && token[i] <= 'F')) return 0;
			}
			return 1;
		case DECIMAL:
			if(token[0] == '0') return 0;
			return validCharactersInRange(token, 0, strlen(token), '0', '9');
		case FLOAT:
			(ePtr = strchr(token, 'e')) || (ePtr = strchr(token, 'E'));
			decPtr = strchr(token, '.');
			if( (!ePtr && !decPtr) ) return 0;
			i = decPtr - token; // i stores the index of the decimal
			if(!decPtr) i = 0; // If there is no decimal, treat it as being at the beginning of the string.	
			j = ePtr - token; // j stores the index of the E sign
			if(!validCharactersInRange(token, ((token[0] == '-') ? 1 : 0), i, '0', '9')) return 0; //Check that everything before the decimal is an integer.
			if(j > 0 && !validCharactersInRange(token, i + 1, j, '0', '9')) return 0; //Check that everything between the decimal and E is an integer.
			if(ePtr != 0){
				if(j + 1 > strlen(token)) return 0; //There's no room for the negative/beginning digit sequence
				if(token[j + 1] == '-' || token[j + 1] == '+'){
					if(j + 2 >= strlen(token)) return 0; // There's no room for the rest of the digit sequence.
					j += 2;
				}else j ++;
				if(!validCharactersInRange(token, j, strlen(token), '0', '9')) return 0; //Check that everything after E+sign is a digit.
			}else{
				return validCharactersInRange(token, i + 1, strlen(token), '0', '9');
			}
			return 1;
		default:
			return 1;
	}
	return 0;
}

Token_Type TKIdentifyToken( char *token){
	if(strlen(token) == 0) return ERROR; // A zero length string, which is an error.

	if(token[0] == '0'){
		if(strlen(token) == 1) return ZERO; // The token is simply "0".
		if(token[1] == 'x' || token[1] == 'X'){
			if(verifyTokenType(token, HEXADECIMAL)) return HEXADECIMAL;
                        return MALFORMED;
		}else if(token[1] == '.'){
			if(verifyTokenType(token, FLOAT)) return FLOAT;
                        return MALFORMED;		
		}else{
			if(verifyTokenType(token, OCTAL)) return OCTAL;
                        return MALFORMED;
		}
	}else{
		if(verifyTokenType(token, DECIMAL)) return DECIMAL;
		if(verifyTokenType(token, FLOAT)) return FLOAT;
               	return MALFORMED;
	}
		
	return ERROR;
}

const char* getTokenTypeName(Token_Type type){
	switch(type){
		case FLOAT: return "Float";
		case DECIMAL: return "Decimal";
		case OCTAL: return "Octal";
		case HEXADECIMAL: return "Hexadecimal";
		case ZERO: return "Zero";
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
 	
	currentToken = "0700 \x02  1234 \n 3.14159e-10 444.5e-e 0 0800 0f55 123 0xFF99 0.5 0.5e-5 0.5e- 0.5e 0.5 .6 5.5.555.5";

//	tokenizer= TKCreate(argv[1]);
	tokenizer= TKCreate(currentToken);
	printf("Tokenizer: %s\n", tokenizer->original);
	
	while((currentToken = TKGetNextToken(tokenizer)) != 0){
		printf("%s: %s\n", getTokenTypeName(TKIdentifyToken(currentToken)), currentToken);
	}
	
	TKDestroy(tokenizer);

	return 0;
}

