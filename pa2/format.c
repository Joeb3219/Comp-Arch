#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define TWO_COMPLEMENT 0
#define ONE_COMPLEMENT 1
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
        }else if(mode == ONE_COMPLEMENT){
		
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

void evaluateInt(char *bits){
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

void floatToString_internal(char *buffer, float number, int power, int numPlaces, int recalculatePower){
	if(number == 0.0 || numPlaces == 0){
		addChar(buffer, 'E');
		if(power < 0){
			addChar(buffer, '-');
			power *= -1;
		}
		char powerBuffer[8];
		sprintf(powerBuffer, "%d\n", power);
		int i;
		for(i = 0; i < 8; i ++){
			if(powerBuffer[i] == '\0') break;
			addChar(buffer, powerBuffer[i]);
		}
		return;
	}
	while(number >= 10.0){
		number /= 10.0;
		if(recalculatePower) power += 1;
	}
	while(number <= 1.0){
		number *= 10.0;
		if(recalculatePower) power -= 1;
	}

	printf("Pushing %d into buffer (%s)\n", '0' + ((int) number), buffer);

	addChar(buffer, '0' + ((int) number));
	if(recalculatePower) addChar(buffer, '.');
	floatToString_internal(buffer, number - ((int) number), power, --numPlaces, 0);
}

void floatToString(char *buffer, float number, int power, int numPlaces){
        floatToString_internal(buffer, number, power, numPlaces, 1);
}


char* evaluateFloat(char *bits){
	float result = 0;
	char *buffer = malloc(32 * sizeof(char));
	int binaryVal = evaluateBinary(bits, VALUE);
	int magnitude, exponent, sign, power = 0, decPlace = 1;
	int whole, numerator, denom;
	
	exponent = binaryVal >> 23 & 0x000000ff;
	magnitude = binaryVal & 0x007fffff;
	magnitude |= 1 << 23;
	sign = binaryVal >> 31;

	int firstBit = firstSetBit(magnitude);
	exponent -= 127;
	decPlace += exponent;
	decPlace = 24 - decPlace;

	if(decPlace > 24) whole = 0;
	else whole =  magnitude >> decPlace;
	denom = decPlace - firstBit;
	numerator = (magnitude >> firstBit) & ((1 << decPlace - firstBit) - 1);

	result = whole + (numerator / pow(2, denom));
	printf("%f\n", result);
	printf("Generating result string\n");
	
	floatToString(buffer, result, 0, 7);

	if(exponent == INF){
		if(magnitude == 0 && sign == 0){
			strcpy(buffer, "+inf");
			return buffer;
		}else if(magnitude == 0 && sign != 0){
			strcpy(buffer, "-inf");
			return buffer;
		}else{
			strcpy(buffer, "NaN");
			return buffer;
		}
	}

	if(sign == -1) addChar(buffer, '-');

	buffer[sizeof(buffer) - 1] = '\0';
	return buffer;	
}

int main( int argv, char ** argc ){
	if(argv != 3) return 1;
	
	char *bits, *format;
	bits = argc[1];
	format = argc[2];

	if(strcmp(format, "int") == 0) evaluateInt(bits);
	else printf("%s\n", evaluateFloat(bits));

	return 0;
}
