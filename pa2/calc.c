#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define PRESERVE 1
#define REPLACE 0

typedef unsigned int uint;

typedef enum base{DEC = 10, BIN = 2, HEX = 16, OCT = 7} Base;

typedef struct number{
	int *representation, digits;
	Base base;
	int negative;
} Number;

Number* add(Number *number1, Number *number2, int mode){
	return number1;
}

/*
 * Takes two numbers and subtracts them by calling number1 + -number2, effectively: number1 - number2.
 */
Number* subtract(Number *number1, Number *number2, int mode){
	number2->negative = (number2->negative) ? 0 : 1;
	return add(number1, number2, mode);
	number2->negative = (number2->negative) ? 0 : 1;
}

Base getBaseByChar(char baseChar){
	if(baseChar == 'b') return BIN;
	if(baseChar == 'd') return DEC;
	if(baseChar == 'h') return HEX;
	if(baseChar == 'o') return OCT;
	return -1;
}

void convertBase(Number *number, Base toBase){
	
}

int charToDig(char c){
	if(c >= '0' && c <= '9') return c - '0';
	if(c >= 'a' && c <= 'f') return c - 'a' + 10;
	else return c - 'A' + 10;
}

char* strrev(char *string){
	int i = 0, j = strlen(string) - 1;
	char *final = malloc(strlen(string) + 1);
	for(i = 0; i < j + 1; i ++){
		final[i] = string[j - i];
	}
	final[strlen(string)] = '\0';
	return final;
}

Number* formNumber(char *representation){
	char *realRepresentation;
	int i;
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
	realRepresentation += 1;
	number->digits = strlen(realRepresentation);
	number->representation = malloc(number->digits * sizeof(int));
	for(i = 0; i < number->digits; i ++){
		number->representation[i] = charToDig(realRepresentation[i]);
	}
	return number;
}

void freeNumber(Number *number){
	free(number);
}

char digToChar(int i){
	if(i < 10) return '0' + i;
	return 'A' + (i - 10);
}

void printNumber(Number *number){
	int i;
	printf("[Num]: %c(%d)", (number->negative == 1) ? '-' : ' ', number->base);
	for(i = 0; i < number->digits; i ++){
		printf("%c", digToChar(number->representation[i]));
	}
	printf("\n");
}

int main(int argv, char **argc){
	if(argv != 5){
		printf("Expected 4 arguments, got %d\n", argv - 1);
		return 1;
	}

	char opsign, format;
	Number *number1, *number2, *result;	
	
	opsign = *argc[1];
	format = *argc[4];
	number1 = formNumber(argc[2]);
	number2 = formNumber(argc[3]);
	if(opsign == '+') result = add(number1, number2, PRESERVE);
	else result = subtract(number1, number2, PRESERVE);

	printNumber(number1);
	printNumber(number2);
	printf("The result (not based): ");
	printNumber(result);
	printf("The result (based): ");
	convertBase(result, getBaseByChar(format)); 
	printNumber(result);

	freeNumber(number1);
	freeNumber(number2);
		
	return 0;
}
