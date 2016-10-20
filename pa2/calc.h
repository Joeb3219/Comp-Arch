#ifndef CALC_H   /* Include guard */
#define CALC_H

typedef unsigned int uint;
typedef unsigned char uchar;

typedef enum base{DEC = 10, BIN = 2, HEX = 16, OCT = 8} Base;

typedef struct number{
        int digits, capacity, negative;
        uchar *rep;
	Base base;
} Number;

char getBaseChar(Base base);
Base getBaseBychar(char baseChar);

void freeNumber(Number *number);
Number* formZeroNumber(Base base);
Number* formNumberFromDec(int num, Base base);
Number* formNumber(char *representation);
Number* copyNumber(Number *reference);

Number* add(Number *number1, Number *number2);
Number* subtract(Number *number1, Number *number2);
Number* mult(Number *number1, Number *nubmer2, Base base);
Number* mult_const(Number *number1, int num);
Number* power(Number *number, Number* times);

void convertBase(Number *number, Base toBase);

uchar getDigit(Number *number, int digit);
void setDigit(Number *number, int digit, uchar num);
void resizeNumber(Number *number);

void removeLeadingZeros(Number *number);
void addZerosBeforeFirstDigit(Number *number, int numZeros);
int isZero(Number *number);
Number* getBiggestNumber(Number *number1, Number *number2);

void numberToASCII(char *buffer, Number *number);
void printNumber(Number *number, int newLine);


#endif // CALC_H

