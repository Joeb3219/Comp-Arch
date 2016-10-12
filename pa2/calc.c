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

Number* formZeroNumber(int base){
	Number *number = malloc(sizeof(Number));
	number->base = base;
	number->digits = 1;
	number->representation = malloc(1 * sizeof(int));
	number->representation[0] = 0;
	return number;
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

Number* multDig(int a, int b, Base base){
	if(a >= base || b >= base) return NULL;
	int product = a * b;
	
	Number *number = malloc(sizeof(Number));
	number->base = base;
	if(product >= base) number->digits = 2;
	else number->digits = 1;
	number->representation = malloc(number->digits * sizeof(int));
	
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

void addDigitsToRepresentation(Number *number, int numDigits){
	int *rep = malloc((number->digits + numDigits) * sizeof(int)), i;
	for(i = 0; i < number->digits + numDigits; i ++) rep[i] = 0;
	for(i = 0; i < number->digits; i ++){
		rep[i + numDigits] = number->representation[i];
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
	if(number1->base != number2->base){
		printf("Bases are not the same: %d, %d\n", number1->base, number2->base);
		return NULL;
	}
	Number *result, *smaller;
	int i, j, maxLength, carry = 0, num1 = 0, num2 = 0, answerNegative;

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

Number* formNumberFromDec(int num, Base base){
	Number *result = malloc(sizeof(Number));
	result->base = base;
	if(num == 0){
		result->digits = 1;
		result->representation = malloc(1 * sizeof(int));
		result->representation[0] = 0;
	}
	result->digits = 0;
	int n = num, i;
	while(n > 0){
		result->digits ++;
		n = (int) n / base;
	}
	result->representation = malloc(sizeof(int) * result->digits);
	for(i = result->digits - 1; i >= 0; i --){
		result->representation[i] = num % base;
		num = (int) num / base;
	}
	return result;
}

Number* mult(Number* number1, Number* number2, Base base){
	Number *result = formZeroNumber(base), *step, *temp, *intermediateNum;
	int carry, intermediate, i, j, k, l;
	
	k = 0;
	for(i = number1->digits - 1; i >= 0; i --){
		step = malloc(sizeof(Number));
		step->base = base;
		step->digits = 1;
		step->representation = malloc(sizeof(int) * 1);
		step->representation[0] = 0;

		carry = 0;
		for(j = number2->digits - 1; j >= 0; j --){
			intermediate = number1->representation[i] * number2->representation[j] + carry;
			carry = (int) intermediate / base;
			intermediate -= (carry * base);
			if(j != 0 || carry == 0){ // We're not at the left side of the equation, so remainders will be carried.
				step->representation[0] = intermediate;
			}else{
				intermediateNum = formNumberFromDec(intermediate + (carry * base), base);
				addDigitsToRepresentation(step, 1);
				step->representation[0] = intermediateNum->representation[0];
				step->representation[1] = intermediateNum->representation[1];
				freeNumber(intermediateNum);	
			}
			addDigitsToRepresentation(step, 1);
			step->representation[0] = 0;
		}
		if(k > 0){
			step->representation = realloc(step->representation, sizeof(int) * (step->digits + k));
			for(l = 0; l < k; l ++){
				step->representation[step->digits + l] = 0;
			}
			step->digits += k;
		}
		temp = add(result, step);
		freeNumber(result);
		freeNumber(step);
		result = temp;
		k ++;
	}
	
	return result;
}

Number* mult_const(Number *number, int num){
	Number *num2 = formNumberFromDec(num, number->base);
	Number *result = mult(number, num2, number->base);
	free(num2);
	return result;
}

void convertBase(Number *number, Base toBase){
        if(number->base == toBase) return;
	Number *result = formZeroNumber(toBase), *intermediate, *temp, *powerFactor;
	int power = 0, i = 0, j = 0, *newRep;
	
	powerFactor = formNumberFromDec(1, toBase);	

	for(i = number->digits - 1; i >= 0; i --){
		intermediate = formNumberFromDec(number->representation[i], toBase);
		for(j = 0; j < power; j ++){
			temp = mult_const(powerFactor, number->base);
			freeNumber(powerFactor);
			powerFactor = temp;
		}
		temp = mult(intermediate, powerFactor, toBase);
		freeNumber(intermediate);
		intermediate = temp;
		temp = add(result, intermediate);
		freeNumber(result);
		freeNumber(intermediate);
		result = temp;
		power ++;
	}

	number->digits = result->digits;
	newRep = malloc(result->digits * sizeof(int));
	for(i = 0; i < number->digits; i ++) newRep[i] = result->representation[i];
	free(number->representation);
	number->representation = newRep;
	number->base = toBase;
	
	freeNumber(powerFactor);
        freeNumber(result);
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

int isZero(Number *number){
	if(number->digits > 1) return 0;
	if(number->digits == 0) return 1;
	if(number->representation[0] == 0) return 1;
	return 0;
}

Number* power(Number *number, Number *times){
	Number *result = formZeroNumber(number->base), *one = formNumberFromDec(1, number->base), *temp, *timesDup = copyNumber(times);
	while(isZero(timesDup) == 0){
		temp = mult(result, number, result->base);
		freeNumber(result);
		result = temp;
		
		//Subtract one from times
		temp = subtract(timesDup, one);
		freeNumber(timesDup);
		timesDup = temp;
	}
	freeNumber(timesDup);
	return result;
}

int main(int argc, char **argv){
	if(argc != 5){
		printf("Expected 4 arguments\n");
		return 0;
	}

	char opsign, format;
	Number *number1, *number2, *result;
	Base targetBase;	

	opsign = *argv[1];
	format = *argv[4];
	number1 = formNumber(argv[2]);
	number2 = formNumber(argv[3]);
	targetBase = getBaseByChar(format);

	if(number1->base != targetBase) convertBase(number1, targetBase);
	if(number2->base != targetBase) convertBase(number2, targetBase);

	if(opsign == '+') result = add(number1, number2);
	else if(opsign == '-') result = subtract(number1, number2);
	else if(opsign == '*') result = mult(number1, number2, number1->base);
	else if(opsign == '^') result = power(number1, number2);

	printNumber(number1);
	printNumber(number2);
	printf("The result: ");
	printNumber(result);

	freeNumber(number1);
	freeNumber(number2);
	freeNumber(result);	
	
	return 0;
}
