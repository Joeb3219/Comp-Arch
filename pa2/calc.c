#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned int uint;

typedef enum base{DEC = 10, BIN = 2, HEX = 16, OCT = 7} Base;

typedef struct number{
	int *representation, digits;
	Base base;
	int negative;
} Number;

char digToChar(int i){
        if(i < 10) return '0' + i;
        return 'A' + (i - 10);
}

void printNumber(Number *number){
        int i, nonZeroFound = 0;
        printf("[Num]: %c(%d)", (number->negative == 1) ? '-' : ' ', number->base);
        for(i = 0; i < number->digits; i ++){
                if(number->representation[i] > 0) nonZeroFound = 1;
                if(nonZeroFound == 1) printf("%c", digToChar(number->representation[i]));
        }
        printf("\n");
}

void freeNumber(Number *number){
	free(number->representation);
        free(number);
}

Number* copyNumber(Number *reference){
	int i;
	Number* result = malloc(sizeof(Number));
	result->representation = malloc(sizeof(int) * reference->digits);
	for(i = 0; i < reference->digits; i ++) result->representation[i] = reference->representation[i];
	result->digits = reference->digits;
	result->base = reference->base;
	result->negative = reference->negative;
	return result;
}

char* mult(char *a, char *b, Base base){
	return NULL;
}

Number* multDig(int a, int b, Base base){
	if(a >= base || b >= base) return NULL;
	int product = a * b;
	
	Number *number = malloc(sizeof(Number));
	number->base = base;
	if(product >= base) number->digits = 2;
	else number->digits = 1;
	number->representation = malloc(number->digits * sizeof(int));
	
	printf("%d * %d is %d\n", a, b, product);

	if(product < 0){
		number->negative = 1;
		product *= -1;
	}
	if(product > base){
		number->representation[0] = (int) product / base;
		product -= ((int) product / base) * base;
		number->representation[1] = product;
	}else number->representation[0] = product;
	return number;
}

void convertBase(Number *number, Base toBase){
	Number *result = copyNumber(number);
	printf("Currently, number is "); printNumber(number);
	printf("Target base is %d\n", toBase);
	
	free(result);	
}

void addDigitsToRepresentation(Number *number, int numDigits){
	int *rep = malloc((number->digits + numDigits) * sizeof(int)), i;
	for(i = 0; i < number->digits + numDigits; i ++){
		if(i > number->digits){
			rep[number->digits + numDigits - i] = 0;
			continue;
		}
		rep[number->digits + numDigits - i] = number->representation[number->digits - i];
	}
	free(number->representation);
	number->representation = rep;
	number->digits += numDigits;
}

Number* getBiggerNumber(Number *number1, Number *number2){
	if(number1->base != number2->base) return NULL;
	int i, j;
	for(i = 0; i < number1->digits; i ++){
		if(number1->representation[i] >= 1) break;
	}
	for(j = 0; j < number2->digits; j ++){
		if(number2->representation[j] >= 1) break;
	}
	if( (number1->digits - i) > (number2->digits - j) ) return number1;
	if( (number1->digits - i) < (number2->digits - j) ) return number2;
	if( number1->representation[i] > number2->representation[j]) return number1;
	return number2;	
}


Number* add(Number *number1, Number *number2){
	Number *result, *smaller;
	int i, j, maxLength, carry = 0, num1 = 0, num2 = 0, answerNegative;


	if(number1->base != number2->base) convertBase(number1, number2->base); // Ensure bases are the same.
	if(getBiggerNumber(number1, number2) == number1){
		result = copyNumber(number1);
		smaller = copyNumber(number2);
	}else{
		result = copyNumber(number2);
		smaller = copyNumber(number1);
	}
	if(number2->negative == number1->negative){
		answerNegative = number1->negative;
		result->negative = smaller->negative = 0;
	}else if(result->negative == 1){
		answerNegative = 1;
		result->negative = 0;
		smaller->negative = 1;
	}else{
		answerNegative = 0;
	}

	if(result->digits < smaller->digits) addDigitsToRepresentation(result, number2->digits - result->digits + 1);
	else addDigitsToRepresentation(result, 1);	

	maxLength = result->digits;

	for(j = 0; j < maxLength; j ++){
		i = maxLength - j - 1;
		num1 = ( (result->negative) ? -1 : 1) * result->representation[i];
		if(smaller->digits > j) num2 = smaller->representation[smaller->digits - j - 1] * ( (smaller->negative) ? -1 : 1);
		else num2 = 0;
		//printf("c%d + %d + %d = %d\n", carry, num1, num2, carry + num1 + num2);
		result->representation[i] = carry + (num1) + (num2);
		if(result->representation[i] < 0){
			// If we are at the left of the equation, we know that the result is negative.
			if(i == 0){
				result->negative = 1;
				break;
			}
			result->representation[i - 1] --;
			result->representation[i] += result->base;
			carry = 0;
		}else if(result->representation[i] > result->base - 1){
			carry = result->representation[i] / result->base;
			result->representation[i] -= (carry * result->base);
		}else carry = 0;
	}
	
	result->negative = answerNegative;
	freeNumber(smaller);
	return result;
}

/*
 * Takes two numbers and subtracts them by calling number1 + -number2, effectively: number1 - number2.
 */
Number* subtract(Number *number1, Number *number2){
	Number *result;
	number2->negative = (number2->negative) ? 0 : 1;
	result = add(number1, number2);
	number2->negative = (number2->negative) ? 0 : 1;
	return result;
}

Base getBaseByChar(char baseChar){
	if(baseChar == 'b') return BIN;
	if(baseChar == 'd') return DEC;
	if(baseChar == 'h') return HEX;
	if(baseChar == 'o') return OCT;
	return -1;
}

int charToDig(char c){
	if(c >= '0' && c <= '9') return c - '0';
	if(c >= 'a' && c <= 'f') return c - 'a' + 10;
	return c - 'A' + 10;
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
	if(opsign == '+') result = add(number1, number2);
	else result = subtract(number1, number2);

	printNumber(number1);
	printNumber(number2);
	printf("The result (not based): ");
	printNumber(result);
	printf("The result (based): ");
	convertBase(result, getBaseByChar(format)); 
	printNumber(result);

	freeNumber(number1);
	freeNumber(number2);
	freeNumber(result);	
	
	return 0;
}
