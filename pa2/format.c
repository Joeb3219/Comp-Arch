#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define TWO_COMPLEMENT 0
#define EXPONENT 3
#define VALUE 2
#define INF 0xFF

int evaluateBinary(char *bits, int mode){
        int val = 0, n = strlen(bits), i, oneFound = 0;

        if(mode == TWO_COMPLEMENT){
                if(bits[0] == '0') return evaluateBinary(bits, VALUE);
                for(i = n - 1; i >= 0; i --){
                        if(bits[i] == '1' && !oneFound){
                                oneFound = 1;
                                val += (1 * pow(2, (n - i - 1)));
                                continue;
                        }
                        if(oneFound){
                                if(bits[i] == '0') val += (1 * pow(2, (n - i - 1)));
                        }
                }
                return -val;
        }else if(mode == EXPONENT){
		val = evaluateBinary(bits, VALUE);
		return val - (pow(2, n - 1) - 1);
	}else{
                for(i = n - 1; i >= 0; i --){
                        if(bits[i] == '1') val += (1 * pow(2, (n - i - 1)));
                }
                return val;
        }

        return -1;
}

void addChar( char *string, char c){
	char buffer[2];
	buffer[0] = c;
	buffer[1] = '\0';
	strcat(string, buffer);
}

void evaluateInt(char *buffer, char *bits){
	printf("%d\n", evaluateBinary(bits, TWO_COMPLEMENT));
}

/**
 * Returns the place value of the first bit set to 1 in num
 * If num is 00001 and searching for 1, will return 0 (0 place).
*/
int firstSetBit(int num){
	int place = sizeof(num) * 8 - 1;
	while(place != 0){
		if( (num << place) != 0) return 32 - place - 1;
		place --;
	}
	return -1;
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
	int power = 0, i = 0;
	char powerString[32];
	float precision = 0.00000001;
	printf("Number we're dealing with: %f\n", number);
	if(number < 0){
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
	for(i = 0; i < 7; i ++){
		number -= ((int) number);
		number *= 10.0;
		addChar(buffer, '0' + ((int) number));
	}

	addChar(buffer, 'e');
	intToString(buffer, power);
}

union Number {
	int	i;
	float	f;
};

void evaluateFloat(char *buffer, char *bits){
	float result = 0;
	int binaryVal = evaluateBinary(bits, VALUE);
	int magnitude, exponent, sign, power = 0, decPlace = 1;
	int whole, numerator, denom;
	union Number a;
	a.f = binaryVal;
	
	sign = a.i >> 31;
	exponent = a.i >> 23 & 0x000000ff;
	magnitude = a.i & 0x007fffff;
	magnitude |= 1 << 23;

	int firstBit = firstSetBit(magnitude);
	exponent -= 127;
	decPlace += exponent;
	decPlace = 24 - decPlace;

	printf("M %08x, E %2x, S %1x\n", magnitude, exponent, sign);

	if(decPlace > 24) whole = 0;
	else whole =  magnitude >> decPlace;
	denom = decPlace - firstBit;
	numerator = (magnitude >> firstBit) & ((1 << decPlace - firstBit) - 1);

	printf("W %d, N %d, D %d\n", whole, numerator, denom); 

	result = whole + (numerator / pow(2, denom));
	printf("Result before sign: %f\n", result);
	if(sign != 0) result *= -1;

	if(exponent == INF){
		if(magnitude == 0 && sign == 0){
			strcpy(buffer, "+inf");
			return;
		}else if(magnitude == 0 && sign != 0){
			strcpy(buffer, "-inf");
			return;
		}else{
			strcpy(buffer, "NaN");
			return;
		}
	}

	floatToString(buffer, result);

	return;
}

int main( int argv, char ** argc ){
	if(argv != 3) return 1;
	
	char *bits, *format, *buffer = malloc(32 * sizeof(char));
	bits = argc[1];
	format = argc[2];

	if(strcmp(format, "int") == 0) evaluateInt(buffer, bits);
	else evaluateFloat(buffer, bits);

	printf("%s\n", buffer);

	return 0;
}
