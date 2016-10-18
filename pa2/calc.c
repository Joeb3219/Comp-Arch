#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define DEFAULT_NUMBER_REP_ELEM 64
#define LEFT_EXCL 240
#define RIGHT_EXCL 15

typedef unsigned int uint;
typedef unsigned char uchar;

typedef enum base{DEC = 10, BIN = 2, HEX = 16, OCT = 8} Base;

typedef struct number{
	int digits, capacity;
	uchar *rep;
	Base base;
	int negative;
} Number;

char digToChar(int i){
        if(i < 10) return '0' + i;
        return 'A' + (i - 10);
}

uchar getDigit(Number *number, int digit){
	if(digit >= number->digits || digit < 0 || number->digits <= 0) return -1;
	if(digit % 2 == 0) return (number->rep[digit / 2] & 240) >> 4;
	else return number->rep[digit / 2] & 15;
}

void resizeNumber(Number *number){
	number->capacity += DEFAULT_NUMBER_REP_ELEM;
	number->rep = realloc(number->rep, number->capacity * sizeof(uchar));
}

void setDigit(Number *number, int digit, uchar num){
	if(digit >= number->capacity * 2) resizeNumber(number);
	uchar ch, left, right;
	int accessingDigit = (int) digit / 2;
	ch = number->rep[accessingDigit];
	left = LEFT_EXCL & ch;
	right = RIGHT_EXCL & ch;
	if(digit % 2 == 0) number->rep[accessingDigit] = (num << 4) | right;
	else number->rep[accessingDigit] = left | num;
	if(digit >= number->digits) number->digits = digit + 1;
}

char getBaseChar(Base base){
        switch(base){
                case DEC: return 'd';
                case HEX: return 'h';
                case BIN: return 'b';
                case OCT: return 'o';
                default: return '?';
        }
        return '?';
}

void removeLeadingZeros(Number *number){
	int i = 0;
	for(i = number->digits - 1; i >= 0; i --){
		if(getDigit(number, i) != 0){
			number->digits = i + 1;
			break;
		}
	}
}

Base getBaseByChar(char baseChar){
        if(baseChar == 'b') return BIN;
        if(baseChar == 'd') return DEC;
        if(baseChar == 'h') return HEX;
        if(baseChar == 'o') return OCT;
        return -1;
}


void addChar(char *str, char c){
	char buffer[2];
	buffer[0] = c;
	buffer[1] = '\0';
	strcat(str, buffer);
}

void numberToASCII(char *buffer, Number *number){
        int i;

	if(number->negative) addChar(buffer, '-');
	addChar(buffer, getBaseChar(number->base));

	for(i = number->digits - 1; i >= 0; i --){
		addChar(buffer, digToChar(getDigit(number, i)));
	}

}

void printNumber(Number *number, int newLine){
	char *buffer = malloc(sizeof(char) * (number->digits + 3)); // Add 2 extra spaces for base letter and sign.
	buffer[0] = '\0';
	numberToASCII(buffer, number);
	if(newLine == 1) printf("%s\n", buffer);
	else printf("%s", buffer);
	free(buffer);
}

void freeNumber(Number *number){
	int i = 0;
	for(i = 0; i < number->capacity; i ++) number->rep[i] = 1;
	number->rep[number->capacity - 1] = '\0';
	free(number->rep);
        free(number);
}

Number* formZeroNumber(int base){
	Number *number = malloc(sizeof(Number));
	number->base = base;
	number->digits = 1;
	number->negative = 0;
	number->capacity = DEFAULT_NUMBER_REP_ELEM;
	number->rep = malloc(sizeof(uchar) * DEFAULT_NUMBER_REP_ELEM);
	setDigit(number, 0, 0);
	return number;
}

uchar* copyArray(uchar *arr, int size){
        uchar *res = malloc(sizeof(uchar) * size);
        if(size < 1) return res;
	int i;
        for(i = 0; i < size; i ++) res[i] = arr[i];
        return res;
}


Number* copyNumber(Number *reference){
	Number* result = formZeroNumber(reference->base);
	result->digits = reference->digits;
	result->capacity = reference->capacity;
	free(result->rep);
	result->rep = copyArray(reference->rep, reference->capacity);
	result->base = reference->base;
	result->negative = reference->negative;
	return result;
}

Number* getBiggerNumber(Number *number1, Number *number2){
	if(number1->base != number2->base) return NULL;
	if(number1->digits == 0) return number2;
	if(number2->digits == 0) return number1;
	if(number1->digits > number2->digits) return number1;
	if(number2->digits > number1->digits) return number2;

	int i, j;
	for(i = number1->digits - 1; i >= 0; i --){
		if(getDigit(number1, i) >= 1) break;
	}
	for(j = number2->digits - 1; j >= 0; j --){
                if(getDigit(number2, j) >= 1) break;
        }
	if( (number1->digits - i) > (number2->digits - j) || i == number1->digits) return number1;
	if( (number1->digits - i) < (number2->digits - j) || j == number2->digits) return number2;

	if( getDigit(number1, number1->digits - 1) > getDigit(number2, number2->digits - 1)) return number1;
	return number2;
}


Number* add(Number *number1, Number *number2){
	if(number1->base != number2->base){
		printf("Bases are not the same: %d, %d\n", number1->base, number2->base);
		return NULL;
	}
	Number *result, *smaller;
	int  j, carry = 0, num1 = 0, num2 = 0, answerNegative, sum, nextDigNeg = 0;

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

	while(result->digits < smaller->digits){
		setDigit(result, result->digits, 0);
	}
	setDigit(result, result->digits, 0);

	for(j = 0; j < result->digits; j ++){
		num1 = ( (result->negative) ? -1 : 1) * getDigit(result, j) * ((nextDigNeg == 0) ? 1 : -1);
		if(smaller->digits > j) num2 = getDigit(smaller, j) * ( (smaller->negative) ? -1 : 1);
		else num2 = 0;
		sum = carry + num1 + num2;
		if(sum < 0){
			// If we are at the left of the equation, we know that the result is negative.
			if(j == result->digits){
				result->negative = 1;
				setDigit(result, j, sum * -1);
				break;
			}
			if(getDigit(result, j + 1) == 0){
				 nextDigNeg = 1;
				setDigit(result, j + 1, 1);
			}else{
				setDigit(result, j + 1, getDigit(result, j + 1) - 1);
				nextDigNeg = 0;
			}
			sum += result->base;
			setDigit(result, j, sum);
			carry = 0;
		}else if(sum > result->base - 1){
			carry = sum / result->base;
			sum -= (carry * result->base);
		}else{
			carry = 0;
		}
		setDigit(result, j, sum);
	}

	if(result->negative == 0) result->negative = answerNegative;
	freeNumber(smaller);
	removeLeadingZeros(result);
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

int charToDig(char c){
	if(c >= '0' && c <= '9') return c - '0';
	if(c >= 'a' && c <= 'f') return c - 'a' + 10;
	return c - 'A' + 10;
}

Number* formNumberFromDec(int num, Base base){
	Number *result = formZeroNumber(base);
	if(num < 0){
		result->negative = 1;
		num *= -1;
	}else result->negative = 0;
	result->base = base;
	if(num == 0) return result;
	result->digits = 0;
	int n = num, i;
	while(n > 0){
		result->digits ++;
		n = (int) n / base;
	}
	for(i = 0; i < result->digits; i ++){
		setDigit(result, i, (uchar) (num % base));
		num = (int) num / base;
	}
	return result;
}

void addZerosBeforeFirstDigit(Number *number, int numZeros){
	Number *ref = copyNumber(number);
	int i = 0;
	for(i = 0; i < numZeros; i ++){
		setDigit(ref, i, 0);
	}
	for(i = 0; i < number->digits; i ++){
		setDigit(ref, numZeros + i, getDigit(number, i));
	}
	number->digits = ref->digits;
	number->capacity = ref->capacity;
	free(number->rep);
	number->rep = copyArray(ref->rep, ref->capacity);
	freeNumber(ref);
}

Number* mult(Number* number1, Number* number2, Base base){
	Number *result = formZeroNumber(base), *step, *temp, *intermediateNum;
	int carry, intermediate, i, j, k, modDigit = 0;

	k = 0;
	for(i = 0; i < number1->digits; i ++){
		modDigit = 0;
		step = formZeroNumber(base);
		carry = 0;
		for(j = 0; j < number2->digits; j ++){
			intermediate = getDigit(number1, i) * getDigit(number2, j) + carry;
			carry = (int) intermediate / base;
			intermediate -= (carry * base);
			if(j != number2->digits - 1 || carry == 0){ // We're not at the left side of the equation, so remainders will be carried.
				setDigit(step, modDigit ++, intermediate);
			}else{
				intermediateNum = formNumberFromDec(intermediate + (carry * base), base);
				setDigit(step, modDigit ++, getDigit(intermediateNum, 0));
				setDigit(step, modDigit ++, getDigit(intermediateNum, 1));
				freeNumber(intermediateNum);
			}
		}
		if(k > 0){
			addZerosBeforeFirstDigit(step, k);
		}
		temp = add(result, step);
		freeNumber(result);
		freeNumber(step);
		result = temp;
		k ++;
	}

	result->negative = (number1->negative != number2->negative);

	return result;
}

Number* mult_const(Number *number, int num){
	Number *num2 = formNumberFromDec(num, number->base);
	Number *result = mult(number, num2, number->base);
	freeNumber(num2);
	return result;
}

void convertBase(Number *number, Base toBase){
        if(number->base == toBase) return;
	Number *result = formZeroNumber(toBase), *intermediate, *temp, *powerFactor;
	int power = 0, i = 0;

	powerFactor = formNumberFromDec(1, toBase);

	for(i = 0; i < number->digits; i ++){
		intermediate = formNumberFromDec(getDigit(number, i), toBase);
		if(power != 0){
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
	number->capacity = result->capacity;
	free(number->rep);
	number->rep = copyArray(result->rep, number->capacity);
	number->base = toBase;

	freeNumber(powerFactor);
        freeNumber(result);
}

Number* formNumber(char *representation){
	char *realRepresentation;
	int i;

	Number *number = formZeroNumber(DEC);
	if(strlen(representation) == 0) return number;

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
	for(i = 0; i < number->digits; i ++){
		setDigit(number, number->digits - i - 1,  charToDig(realRepresentation[i]));
	}
	return number;
}

int isZero(Number *number){
	int i = 0;
	if(number->digits == 0) return 1;
	for(i = number->digits - 1; i >= 0; i--){
		if(getDigit(number, i) != 0) return 0;
	}
	return 1;
}

Number* power(Number *number, Number *times){
	Number *result = formNumberFromDec(1, number->base), *one = formNumberFromDec(1, times->base), *temp, *timesDup = copyNumber(times);
	int i = 0;

	while(isZero(timesDup) == 0){
		i++;
		temp = mult(number, result, result->base);
		freeNumber(result);
		result = temp;

		//Subtract one from times
		temp = subtract(timesDup, one);
		freeNumber(timesDup);
		timesDup = temp;
	}
	freeNumber(one);
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

	printNumber(number1 , 0);
	printf(" %c ", opsign);
	printNumber(number2, 0);
	printf(" :\n");
	printNumber(result, 1);

	freeNumber(number1);
	freeNumber(number2);
	freeNumber(result);

	return 0;
}
