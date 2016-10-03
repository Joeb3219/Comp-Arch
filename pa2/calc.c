#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define PRESERVE 1
#define REPLACE 0

typedef enum base{DEC = 10, BIN = 2, HEX = 16, OCT = 7} Base;

typedef struct number{
	char *representation;
	Base base;
	int negative;
} Number;

/*
 * Multiplies a number by negative one to invert it.
 * Given d10, will return -d10.
 */
Number* invert(Number *number, int mode){
        return number;
}

Number* add(Number *number1, Number *number2, int mode){
	return number1;
}

/*
 * Takes two numbers and subtracts them by calling number1 + invert(number2), effectively: number1 - number2.
 */
Number* subtract(Number *number1, Number *number2, int mode){
	return add(number1, invert(number2, PRESERVE), PRESERVE);
}

Base getBaseByChar(char baseChar){
	if(baseChar == 'b') return BIN;
	if(baseChar == 'd') return DEC;
	if(baseChar == 'h') return HEX;
	if(baseChar == 'o') return OCT;
	return -1;
}

Number* formNumber(char *representation){
	char *realRepresentation;
	if(strlen(representation) == 0) return NULL;
	Number *number = malloc(sizeof(Number));	
	if(representation[0] == '-'){
		number->negative = 1;
		 realRepresentation = representation + 1; // Skip the sign, we will refer back to it later.
	}else{
		number->negative = 0;
		 realRepresentation = representation;
	}
	number->base = getBaseByChar(realRepresentation[0]);
	number->representation = realRepresentation + 1;
	return number;
}

void freeNumber(Number *number){
	free(number);
}

void printNumber(Number *number){
	printf("[Num]: %c%s(%d)\n", (number->negative == 1) ? '-' : ' ',number->representation, number->base);
}

int main(int argv, char **argc){
	if(argv != 5){
		printf("Expected 4 arguments, got %d\n", argv - 1);
		return 1;
	}
	
	char *opsign, *format;
	Number *number1, *number2;	
	
	opsign = argc[1];
	format = argc[4];
	number1 = formNumber(argc[2]);
	number2 = formNumber(argc[3]);

	printNumber(number1);
	printNumber(number2);

	freeNumber(number1);
	freeNumber(number2);
	
	return 0;
}
