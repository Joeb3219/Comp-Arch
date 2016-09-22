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
	char *original; // The entire input that needs to be tokenized.
	int currentPos; // Used to extract the next token, currentPos holds the value of the index last processed.
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
 * If the passed string pointer is null, TKCreate returns NULL, otherwise returns a TokenizerT.
 *
 */
TokenizerT *TKCreate( char * ts ) {
	if(ts == 0) return NULL; // If we have a null string pointer, we return NULL - there is no tokens to process.
	TokenizerT *token = malloc(sizeof(TokenizerT));
	token->original = strdup(ts); // Duplicates the original string so that the original can be changed/disposed of as needed.
	token->currentPos = 0; // Set the current pos to 0 as we don't know what's in the memory block.
	return token;
}

/*
 * TKDestroy destroys a TokenizerT object.  It should free all dynamically
 * allocated memory that is part of the object being destroyed.
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

	// Iterate through the token, starting at the index last left on until we find a non-escape character.
	// Once we find a character > 0x20, mark beginningIndex at the current position.
	// Continue iterating until we find another escape-character (/n included), at which point the token is between indexes i and beginningIndex.
	for(i = tk->currentPos; i < strlen(tk->original); i ++){
		c = tk->original[i];
		if(c <= 0x20){ // All characters <= 0x20 are escaped characters.
			// If the character is not "whitespace", it must be an error - print it in hex format.
			if(!(c == 0x20 || (c >= 0x09 && c <= 0x0d))) printf("[%#x]\n", tk->original[i]);
			// If beginningIndex is -1, we haven't found a leftbound of the token.
			if(beginningIndex == -1) continue;
			break;
		}else if(beginningIndex == -1) beginningIndex = i; //Leftbound of the token is at i.
	}
	
	if(beginningIndex == -1 || i - beginningIndex <= 0) return 0; //There are no more tokens left!
	tk->currentPos = i; // When attempting to find the next token, we will begin at the current index, i.
	
	// token will be a substring of tk->original, which begins at beginningIndex and ends at i.
	token = (char*) malloc(sizeof(char) * (i - beginningIndex + 1)); 
	strncpy(token, &tk->original[beginningIndex], (i - beginningIndex)); // Copy the first (i - begInd) characters starting at the beginningIndex'th character.
	return token;

}

/**
 * Detects if all of the characters in a string, beginning at index beginningIndex and ending at endingIndex, are within the range {beginningValid, endingValid}.
 * If all of the characters in the index range are within the chracter range, returns 1. Otherwise returns 0.
*/
int validCharactersInRange( char *token, int beginningIndex, int endingIndex, int beginningValid, int endingValid){
	int i = 0;
	for(i = beginningIndex; i < endingIndex; i ++){
		if(!(token[i] >= beginningValid && token[i] <= endingValid)) return 0;
	}
	return 1;
}

/**
 * verifyTokenType will return 1 if a given token fits all of the criteria to be of the specified type.
*/
int verifyTokenType( char *token, Token_Type type){
	int i = 0, j = 0;
	char *ePtr, *decPtr;
	switch(type){
		case OCTAL:
			// OCTAL numbers must begin with 0 and have all numbers after be 0 thru 7, inclusive.
			if(strlen(token) < 2 || token[0] != '0') return 0; // If the len is < 2, can't possibly contain 0 and a digit sequence.
			return validCharactersInRange(token, 0, strlen(token), '0', '7');
		case HEXADECIMAL:
			// HEXADECIMAL numbers must begin with 0X/0x, and follow by only 0 thru F, inclusive.
			if(strlen(token) < 3 || (token[0] != '0')) return 0; // If the len is < 3, can't possibly have 0x followed by a char sequence.
			if(token[1] != 'x' && token[1] != 'X') return 0;
			for(i = 2; i < strlen(token); i ++){
				// If token[i] isn't a digit or a-f or A-F, return 0.
				if(!(token[i] >= '0' && token[i] <= '9') && !(token[i] >= 'a' && token[i] <= 'f') && !(token[i] >= 'A' && token[i] <= 'F')) return 0;
			}
			return 1;
		case DECIMAL:
			// DECIMAL numbers must consist of digits 0-9, except for in the first index, which may be only 1-9.
			if(token[0] == '0') return 0;
			return validCharactersInRange(token, 0, strlen(token), '0', '9');
		case FLOAT:
			// FLOAT numbers require a host of checks, most of which make me want to cry.
			// Our algorithm essentially finds the index of the first e/E char, and the index of the first decimal point.
			// If a e/E char or a decimal char doesn't exist, we simply move our indexes to the right/left, respectively.
			// If the string contains neither, it isn't a float.
			// Once we have the bounds, we can check that all characters to the left of the decimal index are 0-9.
			// We then check that all characters between the decimal & e index are also 0-9.
			// If there is an e character, we ensure that there is a valid sequence following e: +digitSequence, -digitSequence, or simply
			// digitSequence, where digitSequence is all characters 0 thru 9.
			(ePtr = strchr(token, 'e')) || (ePtr = strchr(token, 'E')); // Get the first occurance of e/E.
			decPtr = strchr(token, '.'); // Get the first occurance of a decimal.
			if( (!ePtr && !decPtr) ) return 0; // If neither decimal or e/E in the token, it's not a float.
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

/**
 * Returns the type of the token passed to the function.
 * We make predictions based on the first several characters via elimination, and then verify our results.
 * If we fail to identify the token we return ERROR, or MALFORMED if the type we guessed wasn't valid.
*/
Token_Type TKIdentifyToken( char *token){
	if(strlen(token) == 0) return ERROR; // A zero length string, which is an error.

	// If the first character is 0, our input either looks like: 0123 (octal), 0x123 (hex), or 0.123 (float).
	if(token[0] == '0'){
		if(strlen(token) == 1) return ZERO; // The token is simply "0".
		// 0x or 0X denotes HEXADECIMAL
		if(token[1] == 'x' || token[1] == 'X'){
			if(verifyTokenType(token, HEXADECIMAL)) return HEXADECIMAL;
                        return MALFORMED;
		}else if(token[1] == '.'){ // A decimal denotes that we're dealing with a float.
			if(verifyTokenType(token, FLOAT)) return FLOAT;
                        return MALFORMED;		
		}else{ // If there's no decimal or 0X/0x, we know that this must be an octal number.
			if(verifyTokenType(token, OCTAL)) return OCTAL;
                        return MALFORMED;
		}
	}else{
		// Process of elimination - if it isn't a decimal, it must be a float.
		if(verifyTokenType(token, DECIMAL)) return DECIMAL;
		if(verifyTokenType(token, FLOAT)) return FLOAT;
               	return MALFORMED;
	}
		
	return ERROR; // So sad, we're so close yet so far.
}

/**
 * Converts a type to a string representation.
*/
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
	if(argc == 1) return 1;
	char *currentToken;
	TokenizerT *tokenizer;
 	
	tokenizer= TKCreate(argv[1]);
	
	while((currentToken = TKGetNextToken(tokenizer)) != 0){
		printf("%s: %s\n", getTokenTypeName(TKIdentifyToken(currentToken)), currentToken);
	}
	
	TKDestroy(tokenizer);

	return 0;
}

