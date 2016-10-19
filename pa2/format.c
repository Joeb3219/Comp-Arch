#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "format.h"

#define INF 0xFF

int evaluateBinary(char *bits){
        int val = 0;
	int n = strlen(bits), i;

	for(i = 0; i < n; i ++){
		val = val << 1;
		if(bits[i] == '1') val |= 1;
        }
        return val;
}

void addChar( char *string, char c){
	char buffer[2];
	buffer[0] = c;
	buffer[1] = '\0';
	strcat(string, buffer);
}

void intToString(char *buffer, int number){
	if(number == 0){
		addChar(buffer, '0');
		return;
	}
	int numDigits = floor(log10(abs(number))) + 1, i = 0;
	if(number == 0) numDigits = 1;
	char internalBuffer[numDigits + 1];
	internalBuffer[0] = '\0';
	if(number < 0){
		addChar(buffer, '-');
		number *= -1;
	}
	while(number != 0){
		addChar(internalBuffer, '0' + (number % 10));
		number /= 10;
	}
	internalBuffer[numDigits] = '\0';
	for(i = numDigits; i >= 0; i --) addChar(buffer, internalBuffer[i]);
}

void floatToString(char *buffer, float number){
	int rep = 0;
	memcpy(&rep, &number, sizeof(rep));

	int exp = rep >> 23 & 0x000000ff;
	int sign = rep >> 31;
	int mantissa = rep & 0x007fffff;

	if(exp == INF){
		if(mantissa == 0 && sign != 0){
			strcpy(buffer, "-inf");
			return;
		}if(mantissa == 0 && sign == 0){
                        strcpy(buffer, "+inf");
                        return;
                }if(mantissa != 0 && sign != 0){
                        strcpy(buffer, "-NaN");
                        return;
                }if(mantissa != 0 && sign == 0){
                        strcpy(buffer, "+NaN");
                        return;
                }
	}

	int power = 0, i = 0;
	if(sign != 0){
		number *= -1;
		addChar(buffer, '-');
	}
	while(number >= 10.0){
		number /= 10.0;
                power += 1;
	}
	while(number < 1.0){
        	number *= 10.0;
        	power -= 1;
		if(power <= -7){
			power = 0;
			break;
		}
	}
	addChar(buffer, '0' + ((int) number));
	addChar(buffer, '.');
	for(i = 0; i < 6; i ++){
		number -= ((int) number);
		number *= 10.0;
		addChar(buffer, '0' + ((int) number));
	}

	addChar(buffer, 'e');
	intToString(buffer, power);
}

void evaluateFloat(char *buffer, char *bits){
	int binaryVal = evaluateBinary(bits);
	float val = 0;
	memcpy(&val, &binaryVal, sizeof(float));
	floatToString(buffer, val);
}

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
