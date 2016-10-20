#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "calc.h"
#include "strutil.h"
#define DEFAULT_NUMBER_REP_ELEM 64
#define LEFT_EXCL 240
#define RIGHT_EXCL 15

/*
 * Storing mechanism for digits in a number:
 * uchars are 8 bits, so we can store 2 numbers between 0 and 15.
 * Digit 0 and 1 will both be stored in the first element (i=0) of the array.
 * Digit 0 will be the high bits & Digit 1 will be the low bits.
 * In effect, if digit 0 is 15 and digit 1 is 0, the character will be: 11110000, ergo 240.
 * Hence, ch&240 will yield the left digit (even digit), and ch&15 will yield the right (odd digit)
 */


/*
 * Given a Number, and an integer less than (Number->capacity * 2), will return the digit at the indicated position of the Number.
 * If the integer is greater than (Number->capacity * 2), will return -1.
 */
uchar getDigit(Number *number, int digit){
	if(digit >= number->digits || digit < 0 || number->digits <= 0) return -1; // If digit outside of [0, number->digits), the digit doesn't exist.
	if(digit % 2 == 0) return (number->rep[digit / 2] & 240) >> 4; // Access the left side and shift over 4
	else return number->rep[digit / 2] & 15; // Access the right side
}

/*
 * Given a Number, will increase the Number's capacity by DEFAULT_NUMBER_REP_ELEM.
 */
void resizeNumber(Number *number){
	number->capacity += DEFAULT_NUMBER_REP_ELEM;
	number->rep = realloc(number->rep, number->capacity * sizeof(uchar));
}

/*
 * Sets the digit'th digit of Number to num.
 * num Must be less than or equal to 15 or it will interfere with the other digit stored in its block.
 * If digit is greater than (number->capacity * 2), the Number will be resized to accomodate.
 */
void setDigit(Number *number, int digit, uchar num){
	if(digit >= number->capacity * 2) resizeNumber(number); // If the internal array cannot hold digit digits, expand the number.
	uchar ch, left, right;
	int accessingDigit = (int) digit / 2; // Regardless of odd or even, the index of a digit is the integer of digit/2. Ergo, 0 and 1 -> 0, 8 and 9 -> 4, etc.
	ch = number->rep[accessingDigit];
	left = LEFT_EXCL & ch; // Store the left number.
	right = RIGHT_EXCL & ch; // Store the right number.
	if(digit % 2 == 0) number->rep[accessingDigit] = (num << 4) | right; // Setting the left side: shift the number to the left (so it's xxxx0000), | right ensures that right is unchanged.
	else number->rep[accessingDigit] = left | num; // Setting the right side: (0000xxxx), | left ensures that left is unchanged.
	if(digit >= number->digits) number->digits = digit + 1;
}

/*
 * Given a Base b, will return the char representation of that Base.
 * EG: If b is HEX, will return h, if b is OCT, will return o.
 */
char getBaseChar(Base base){
        switch(base){
                case DEC: return 'd';
                case HEX: return 'h';
                case BIN: return 'b';
                case OCT: return 'o';
                default: return '?'; // Gotta keep them guessing
        }
        return '?';
}

/*
 * Moves the digits value of a Number to the first nonzero value such that 01234 becomes 1234.
 * This operation doesn't change the value of a number.
 */
void removeLeadingZeros(Number *number){
	int i = 0;
	// Iterates through digits from most significant to least significant until hits a nonzero number.
	// We then set the number's digits to that index + 1 (ie: if number is 0110, will set digits to 3).
	for(i = number->digits - 1; i >= 0; i --){
		if(getDigit(number, i) != 0){
			number->digits = i + 1;
			break;
		}
	}
}

/*
 * Returns the Base associated with a character.
 * getBaseChar(getBaseByChar(x)) should return x.
 */
Base getBaseByChar(char baseChar){
        if(baseChar == 'b') return BIN;
        if(baseChar == 'd') return DEC;
        if(baseChar == 'h') return HEX;
        if(baseChar == 'o') return OCT;
        return -1;
}

/*
 * Converts a Number to an ASCII representation, stuffing it into the provided buffer, which is assumed to be large enough.
 */
void numberToASCII(char *buffer, Number *number){
        int i;

	if(number->negative) addChar(buffer, '-');
	addChar(buffer, getBaseChar(number->base));

	for(i = number->digits - 1; i >= 0; i --){
		addChar(buffer, digToChar(getDigit(number, i)));
	}

}

/*
 * Prints out a Number, number. Allocates its own buffer of the correct size and then handles printing.
 * IF newline is 1, will output \n at the end of the string.
 */
void printNumber(Number *number, int newLine){
	char *buffer = malloc(sizeof(char) * (number->digits + 3)); // Add 2 extra spaces for base letter and sign.
	buffer[0] = '\0';
	numberToASCII(buffer, number);
	if(newLine == 1) printf("%s\n", buffer);
	else printf("%s", buffer);
	free(buffer);
}

/*
 * Frees the Number and all of its data
 */
void freeNumber(Number *number){
	free(number->rep); // Free the alloc'd uchar array
        free(number);
}

/*
 * Forms a number in Base b, which has a value equal to zero.
 * This should be called whenver a number is made, as it sets all of the data needed to ensure no random data stored.
 */
Number* formZeroNumber(Base base){
	Number *number = malloc(sizeof(Number));
	number->base = base;
	number->digits = 1;
	number->negative = 0;
	number->capacity = DEFAULT_NUMBER_REP_ELEM;
	number->rep = malloc(sizeof(uchar) * DEFAULT_NUMBER_REP_ELEM);
	setDigit(number, 0, 0);
	return number;
}

/*
 * Copies a number, verbatim, and returns a duplicate.
 * Copies the internal array instead of duplicating its memory address.
 */
Number* copyNumber(Number *reference){
	Number* result = formZeroNumber(reference->base); // Ensures all fields are at least instantiated.
	result->digits = reference->digits;
	result->capacity = reference->capacity;
	free(result->rep); // formZeroNumber creates an internal array by default, but we want to replace it with a new one.
	result->rep = copyArray(reference->rep, reference->capacity);
	result->base = reference->base;
	result->negative = reference->negative;
	return result;
}

/*
 * Given two numbers, number1 and number2, will return which number has a bigger absolute value.
 * IF the bases of the numbers aren't the same, will return NULL.
 */
Number* getBiggerNumber(Number *number1, Number *number2){
	if(number1->base != number2->base) return NULL;
	if(number1->digits == 0) return number2; // If there are no digits, the number isn't even 0.
	if(number2->digits == 0) return number1; // If there are no digits, the number isn't even 0.
//	if(number1->digits > number2->digits) return number1;
//	if(number2->digits > number1->digits) return number2;

	// We can't directly compare the number of digits. 0012 < 123 despite having more digits.
	int i, j;
	for(i = number1->digits - 1; i >= 0; i --){
		if(getDigit(number1, i) >= 1) break; // Find first nonzero digit of number 1
	}
	for(j = number2->digits - 1; j >= 0; j --){
                if(getDigit(number2, j) >= 1) break; // Find first nonzero digit of number 2
	}

	// We can compare the number of nonzero digits, assuming the bases are the same.
	if( i > j || j == -1) return number1;
	if( i < j || i == -1) return number2;

	if( getDigit(number1, number1->digits - 1) > getDigit(number2, number2->digits - 1)) return number1;
	return number2;
}

/*
 * Adds two numbers, number1 and number2, and returns the result. They can be in any base so long as number1->base=number2->base.
 * If the bases are not equal, will return NULL.
 */
Number* add(Number *number1, Number *number2){
	if(number1->base != number2->base){
		fprintf(stderr, "Bases are not the same: %d, %d\n", number1->base, number2->base);
		return NULL;
	}
	Number *result, *smaller;
	int  j, carry = 0, num1 = 0, num2 = 0, answerNegative, sum, nextDigNeg = 0;

	// For simplicity sake, we determine which number has a larger magnitude and use it for adding.
	// This is advantagous for when we have a - b, while a is less than b.
	if(getBiggerNumber(number1, number2) == number1){
		result = copyNumber(number1);
		smaller = copyNumber(number2);
	}else{
		result = copyNumber(number2);
		smaller = copyNumber(number1);
	}

	if(number2->negative == number1->negative){ // If they have the same sign, the answer will be that sign.
		answerNegative = number1->negative;
		result->negative = smaller->negative = 0;
	}else if(result->negative == 1){ // If the bigger number is negative, the answer will be negative.
		answerNegative = 1;
		result->negative = 0;
		smaller->negative = 1;
	}else{ // Otherwise, the answer will be positive.
		answerNegative = 0;
	}

	// Result should be one digit longer, at least, than the smaller number.
	while(result->digits < smaller->digits){
		setDigit(result, result->digits, 0);
	}
	setDigit(result, result->digits, 0);

	for(j = 0; j < result->digits; j ++){ // Iterate through all digits in result, which will be longer than smaller.
		num1 = ( (result->negative) ? -1 : 1) * getDigit(result, j) * ((nextDigNeg == 0) ? 1 : -1);
		if(smaller->digits > j) num2 = getDigit(smaller, j) * ( (smaller->negative) ? -1 : 1); // If smaller has >= j digits, num2 will be the jth digit.
		else num2 = 0; // Otherwise it is zero.
		sum = carry + num1 + num2;
		if(sum < 0){ // The sum will only be negative if we added a negative number and some other number. Thus, we need to borrow from the left.
			// If we are at the left of the equation, we know that the result is negative.
			if(j == result->digits){
				result->negative = 1;
				setDigit(result, j, sum * -1);
				break;
			}
			if(getDigit(result, j + 1) == 0){ // If the next digit is 0, we will do negative math and set to 1. We use this hack since we can't set digits to -.
				nextDigNeg = 1;
				setDigit(result, j + 1, 1);
			}else{ // Otherwise borrow from the left as normal.
				setDigit(result, j + 1, getDigit(result, j + 1) - 1);
				nextDigNeg = 0;
			}
			sum += result->base;
			setDigit(result, j, sum);
			carry = 0;
		}else if(sum > result->base - 1){ // If sum > the base, we have to carry over to the next place.
			carry = sum / result->base;
			sum -= (carry * result->base);
			nextDigNeg = 0;
		}else{
			nextDigNeg = 0;
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

/*
 * Creates a Number in the indicated base representing the decimal value provided.
 * If provided 16 and base 16, will return a Number with an internal representation of 11.
 */
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

/*
 * Adds zeros before the first digit. Effectively multiplies the number by base^numZeros.
 * EG: If provided a number, 123 and numZeros = 4, will convert Number to 1230000.
 */
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

/*
 * Multiplies two numbers, number1 and number2, in the indicated base. 
 */
Number* mult(Number* number1, Number* number2, Base base){
	Number *result = formZeroNumber(base), *step, *temp, *intermediateNum;
	int carry, intermediate, i, j, leadingZeros, modDigit = 0;

	leadingZeros = 0;
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
		if(leadingZeros > 0){
			addZerosBeforeFirstDigit(step, leadingZeros);
		}
		temp = add(result, step);
		freeNumber(result);
		freeNumber(step);
		result = temp;
		leadingZeros ++;
	}

	result->negative = (number1->negative != number2->negative);

	return result;
}

/*
 * A wrapper function that will convert num to a Number representing the value of num, and multiply it by number and return the result.
 */
Number* mult_const(Number *number, int num){
	Number *num2 = formNumberFromDec(num, number->base);
	Number *result = mult(number, num2, number->base);
	freeNumber(num2);
	return result;
}

/*
 * Converts the base of a Number from Number->base to toBase.
 */
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

/*
 * Forms a Number from an ASCII character representation.
 * The character string passed is expected to contain the base type (b/o/d/h) and a negative sign if the number is negative.
 */
Number* formNumber(char *representation){
	char *realRepresentation;
	int i, digit;

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
		digit = charToDig(realRepresentation[i]);
		if(digit > number->base || digit < 0){
			fprintf(stderr, "%c is an invalid character for base %d\n", realRepresentation[i], number->base);
			return NULL;
		}
		setDigit(number, number->digits - i - 1,  digit);
	}
	return number;
}

/*
 * If the number represented by Number is zero, will return 1. Otherwise, returns 0.
 */
int isZero(Number *number){
	int i = 0;
	if(number->digits == 0) return 1;
	for(i = number->digits - 1; i >= 0; i--){
		if(getDigit(number, i) != 0) return 0;
	}
	return 1;
}

/*
 * Raises a Number, number, to a power, times. Effectively, computes number*number times times.
 */
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
                fprintf(stderr, "Expected 4 arguments\n");
                return 1;
        }

        char opsign, format;
        Number *number1, *number2, *result;
        Base targetBase;

        opsign = *argv[1];
        format = *argv[4];
        number1 = formNumber(argv[2]);
        number2 = formNumber(argv[3]);
        targetBase = getBaseByChar(format);

	if(targetBase != HEX && targetBase != DEC && targetBase != BIN && targetBase != OCT){
		fprintf(stderr, "Unexpected base %c. Expected either d, o, h, or b.\n", format);
		return 1;
	}

        if(number1 == NULL || number2 == NULL) return 1;

        if(number1->base != targetBase) convertBase(number1, targetBase);
        if(number2->base != targetBase) convertBase(number2, targetBase);

        if(opsign == '+') result = add(number1, number2);
        else if(opsign == '-') result = subtract(number1, number2);
        else if(opsign == '*') result = mult(number1, number2, number1->base);
        else if(opsign == '^') result = power(number1, number2);
	else{
		fprintf(stderr, "Unexpected opsign: %c, expected either +, -, *, or ^\n", opsign);
		return 1;
	}

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
