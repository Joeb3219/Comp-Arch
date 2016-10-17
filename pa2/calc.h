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

char digToChar(int i);
int charToDig(char c);
uchar getDigit(Number *number, int digit);
void resizeNumber(Number *number);
void setDigit(Number *number, int digit, uchar num);
char getBaseChar(Base base);
void removeLeadingZeros(Number *number);
Base getBaseBychar(char baseChar);

void addChar(char *buffer, char c);
void numberToASCII(char *buffer, Number *number);
void printNumber(Number *number);
uchar* copyArray(uchar *arr, int size);

void freeNumber(Number *number);
Number* formZeroNumber(int base);
Number* formNumberFromDec(int num, Base base);
Number* formNumber(char *representation);
Number* copyNumber(Number *reference);

Number* getBiggestNumber(Number *number1, Number *number2);
Number* add(Number *number1, Number *number2);
Number* subtract(Number *number1, Number *number2);

void addZerosBeforeFirstDigit(Number *number, int numZeros);

Number* mult(Number *number1, Number *nubmer2, Base base);
Number* mult_const(Number *number1, int num);
void convertBase(Number *number, Base toBase);

int isZero(Number *number);

Number* power(Number *number, Number* times);

#endif // CALC_H

