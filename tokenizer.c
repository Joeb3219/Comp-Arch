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
typedef enum {DECIMAL = -1, OCTAL = -2, HEXADECIMAL = -3, FLOAT = -4, ZERO = -5, ERROR = -6, MALFORMED = 0} Token_Type;

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
	strncpy(token, &tk->original[beginningIndex], (i - beginningIndex + 1)); // Copy the first (i - begInd) characters starting at the beginningIndex'th character.
	token[i - beginningIndex] = '\0';
	return token;

}

/**
 * Detects if all of the characters in a string, beginning at index beginningIndex and ending at endingIndex, are within the range {beginningValid, endingValid}.
 * If all of the characters in the index range are within the chracter range, returns -1. Otherwise returns the index at which begins being invalid.
*/
int validCharactersInRange( char *token, int beginningIndex, int endingIndex, int beginningValid, int endingValid){
	int i = 0;
	for(i = beginningIndex; i < endingIndex; i ++){
		if(!(token[i] >= beginningValid && token[i] <= endingValid)) return i;
	}
	return -1;
}

/**
 * verifyTokenType will return -1 if a given token fits all of the criteria to be of the specified type, or the index at which it stops being valid otherwise.
*/
int verifyTokenType( char *token, Token_Type type){
	if(token == 0 || strlen(token) == 0) return 0;
	int i = 0, j = 0;
	char *ePtr, *decPtr;
	switch(type){
		case OCTAL:
			// OCTAL numbers must begin with 0 and have all numbers after be 0 thru 7, inclusive.
			if(strlen(token) == 1 || token[0] != '0') return 0; // If the len is < 2, can't possibly contain 0 and a digit sequence.
			return validCharactersInRange(token, 0, strlen(token), '0', '7');
		case HEXADECIMAL:
			// HEXADECIMAL numbers must begin with 0X/0x, and follow by only 0 thru F, inclusive.
			if(strlen(token) < 3) return strlen(token) - 1; // If the len is < 3, can't possibly have 0x followed by a char sequence.
			if(token[0] != '0') return 0; // If the first character isn't 0, it isn't hex.
			if(token[1] != 'x' && token[1] != 'X') return 1; //If the second character isn't x/X, it isn't hex.
			for(i = 2; i < strlen(token); i ++){
				// If token[i] isn't a digit or a-f or A-F, return 0.
				if(!(token[i] >= '0' && token[i] <= '9') && !(token[i] >= 'a' && token[i] <= 'f') && !(token[i] >= 'A' && token[i] <= 'F')) return i;
			}
			return -1;
		case DECIMAL:
			// DECIMAL numbers must consist of digits 0-9, except for in the first index, which may be only 1-9.
			if(token[0] == '0') return 0; // The first character is zero, which isn't allowed in decimals.
			return validCharactersInRange(token, 0, strlen(token), '0', '9'); 
		case FLOAT:
			/* FLOAT numbers require a host of checks, most of which make me want to cry.
			 * Our algorithm essentially finds the index of the first e/E char, and the index of the first decimal point.
			 * If a e/E char or a decimal char doesn't exist, we simply move our indexes to the right/left, respectively.
			 * If the string contains neither, it isn't a float.
			 * Once we have the bounds, we can check that all characters to the left of the decimal index are 0-9.
			 * We then check that all characters between the decimal & e index are also 0-9.
			 * If there is an e character, we ensure that there is a valid sequence following e: +digitSequence, -digitSequence, or simply
			 * digitSequence, where digitSequence is all characters 0 thru 9.
			*/
			ePtr = strchr(token, 'e'); // Get the first occurance of e.
			if(!ePtr) ePtr = strchr(token, 'E'); // If e isn't found, get first occurance of E.
			decPtr = strchr(token, '.'); // Get the first occurance of a decimal.
			if( (!ePtr && !decPtr) ) return 0; // If neither decimal or e/E in the token, it's not a float.
			
			i = decPtr - token; // i stores the index of the decimal 
			if(!decPtr) i = 0; // If there is no decimal, treat it as being at the beginning of the string.	
			j = ePtr - token; // j stores the index of the E sign
			
			int invalidCharIndex = validCharactersInRange(token, ((token[0] == '-') ? 1 : 0), i, '0', '9');
			if(invalidCharIndex != -1) return invalidCharIndex; //Check that everything before the decimal is an integer.
			invalidCharIndex = validCharactersInRange(token, i + 1, j, '0', '9');
			if(j > 0 && invalidCharIndex != -1) return invalidCharIndex; //Check that everything between the decimal and E is an integer.
			if(ePtr != 0){
				if(j + 1 >= strlen(token)) return j; //There's no room for the negative/beginning digit sequence
				if(token[j + 1] == '-' || token[j + 1] == '+'){
					if(j + 2 >= strlen(token)) return j + 1; // There's no room for the rest of the digit sequence.
					j += 2;
				}else j ++;
				invalidCharIndex = validCharactersInRange(token, j, strlen(token), '0', '9');
				if(invalidCharIndex != -1) return invalidCharIndex; //Check that everything after E+sign is a digit.
			}else{
				return validCharactersInRange(token, i + 1, strlen(token), '0', '9');
			}
			return -1;
		default:
			return -1;
	}
	return 0;
}

/**
 * Returns the type of the token passed to the function.
 * We make predictions based on the first several characters via elimination, and then verify our results.
 * If we fail to identify the token we return ERROR, or MALFORMED if the type we guessed wasn't valid.
*/
Token_Type TKIdentifyToken( char *token){
	Token_Type type = 0;
	if(strlen(token) == 0) return ERROR; // A zero length string, which is an error.

	// If the first character is 0, our input either looks like: 0123 (octal), 0x123 (hex), or 0.123 (float).
	if(token[0] == '0'){
		if(strlen(token) == 1) return ZERO; // The token is simply "0".
		// 0x or 0X denotes HEXADECIMAL
		if(token[1] == 'x' || token[1] == 'X'){
			type = verifyTokenType(token, HEXADECIMAL);
			return ((type == -1) ? HEXADECIMAL : type);
		}else if(token[1] == '.'){ // A decimal denotes that we're dealing with a float.
			type = verifyTokenType(token, FLOAT);
                        return ((type == -1) ? FLOAT : type);
		}else{ // If there's no decimal or 0X/0x, we know that this must be an octal number.
			type = verifyTokenType(token, OCTAL);
                        return ((type == -1) ? OCTAL : type);
		}
	}else{
		// Process of elimination - if it isn't a decimal, it must be a float.
		type = verifyTokenType(token, DECIMAL);
		if(type == -1) return DECIMAL;
		else{
			// We need to determine if the faulty character was a decimal/e/E.
			// If it wasn't, then we know we just had a malformed decimal and not a float.
			if((token[type] != '.' && token[type] != 'E' && token[type] != 'e')) return type;
		}
		type = verifyTokenType(token, FLOAT);
		return ((type == -1) ? FLOAT : type);
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
		case ZERO: return "Zero (Decimal)";
		case MALFORMED: return "Malformed";
		default: return "[Err]";
	}
}

void processToken(char *token){
	int identifyToken;
	char *substr;
	if(token == 0 || strlen(token) == 0) return; // The token is empty, let's stop.
	identifyToken = TKIdentifyToken(token);
	if(identifyToken >= 0){
		// Break apart token and call processToken on its parts.
		if(identifyToken > 0){
			substr = malloc( (identifyToken + 1) * sizeof(char)); // Create enough space to hold the first identifyToken characters plus an additional \0.
			strncpy(substr, token, identifyToken + 1);
			substr[identifyToken] = '\0';
			processToken(substr);
			free(substr);			
		}
		// Identify that this token is an error.
		printf("[Err] [%#x]\n", token[identifyToken]);
		// Identify the right part of the string
		if(identifyToken != strlen(token) + 1){
			substr = malloc( (strlen(token) - identifyToken) * sizeof(char)); // Create enough space to hold the first identifyToken characters plus an additional \0.
                        strncpy(substr, &token[identifyToken + 1], strlen(token) - identifyToken);
                        substr[strlen(token) - identifyToken - 1] = '\0';
                        processToken(substr);
                        free(substr);
		}
		return;
	}else if(identifyToken == ERROR){
		printf("[Err] [%#x]\n", token[0]);
	}else printf("%s: %s\n", getTokenTypeName(identifyToken), token);
}

/*
 * main will have a string argument (in argv[1]).
 * The string argument contains the tokens.
 * Print out the tokens in the second string in left-to-right order.
 * Each token should be printed on a separate line.
 */
int main(int argc, char **argv) {
	if(argc == 1) return 1; // If there isn't a token stream passed, return error 1.
	char *currentToken; // String representation of the current token we're processing.
	TokenizerT *tokenizer; // Tokenizer struct
 	
	tokenizer = TKCreate(argv[1]); // Create a tokenizer via TKCreate.
	
	// While TKGetNextToken(tokenizer) doesn't yield 0 (0 = no more tokens), set currentToken to the next token.
	// We then process the token and obtain its type.
	// If in grabbing the next token the TKGetNextToken function encounters an escape sequence, it will handle printing it on its own.
	while((currentToken = TKGetNextToken(tokenizer)) != 0){
		// Print the token's type and text.
		//printf("%s: %s\n", getTokenTypeName(TKIdentifyToken(currentToken)), currentToken);
		processToken(currentToken);
		free(currentToken); // We malloc'd this space, so let's free it now that we're done with it.
	}
	
	// Free the pointer, because we're not bad people.
	TKDestroy(tokenizer);

	// Nothing too bad happened [probably], so we can return 0.
	return 0;
}

