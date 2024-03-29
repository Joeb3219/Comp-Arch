#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "format.h"

/*
 * Interprets an ASCII sequence of characters and converts to an integer
 */
int evaluateBinary(char *bits){
        int val = 0;
	int n = strlen(bits), i;

	for(i = 0; i < n; i ++){
		val = val << 1;
		if(bits[i] == '1') val |= 1;
		else if(bits[i] != '0'){
			fprintf(stderr, "Unexpected character in bit sequence: %c\n", bits[i]);
			exit(1);
		}
        }
        return val;
}

/*
 * Converts a sequence of bits into a float string.
 */
void evaluateFloat(char *buffer, char *bits){
	int binaryVal = evaluateBinary(bits);
	float val = 0;
	memcpy(&val, &binaryVal, sizeof(float));
	floatToString(buffer, val);
}

/*
 * Converts a sequence of bits into an int string.
 */
void evaluateInt(char *buffer, char *bits){
        intToString(buffer, evaluateBinary(bits));
}

int main( int argv, char ** argc ){
	if(argv != 3) return 1;

	char *bits, *format, *buffer = malloc(32 * sizeof(char));
	bits = argc[1];
	format = argc[2];

	if(strlen(bits) != 32){
		fprintf(stderr, "Invalid bit sequence: expected 32 bits but got %d\n", (int) strlen(bits));
		return 1;
	}

	if(strcmp(format, "int") == 0) evaluateInt(buffer, bits);
	else if(strcmp(format, "float") == 0)evaluateFloat(buffer, bits);
	else{
		fprintf(stderr, "Invalid format supplied\n");
		return 1;
	}

	printf("%s\n", buffer);

	return 0;
}
