#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#define TWO_COMPLEMENT 0
#define ONE_COMPLEMENT 1
#define EXPONENT 3
#define VALUE 2

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


void evaluateInt(char *bits){
	printf("%d\n", evaluateBinary(bits, TWO_COMPLEMENT));
}

void evaluateFloat(char *bits){
	char expBits[9], magBits [25];
	int magnitudeDigits[24], magnitude, exponent, sign, i;

	strncpy(expBits, &bits[1], 8);
	expBits[9] = '\0';
	
	magBits[0] = '1';
	strncpy(&magBits[1], &bits[9], 24);
	magBits[25] = '\0';

	for( i = 0; i < (sizeof(magnitudeDigits) / sizeof(magnitudeDigits[0])); i ++) magnitudeDigits[i] = -1;
	while(magnitude != 0){
		magnitudeDigits[--i] = magnitude % 10;
		magnitude /= 10;
	}

	magnitude = evaluateBinary(magBits, VALUE);
	exponent = evaluateBinary(expBits, EXPONENT);
	sign = (bits[0] == '1');	

	printf("%c", (sign == 1) ? '-' : ' ');
	
	printf("%d", magnitudeDigits[i++]);
	printf(".");
	for(i; i < (sizeof(magnitudeDigits) / sizeof(magnitudeDigits[0])); i ++) printf("%d", magnitudeDigits[i]);

	printf("e%d\n", exponent);
}

int main( int argv, char ** argc ){
	if(argv != 3) return 1;
	
	char *bits, *format;
	bits = argc[1];
	format = argc[2];

	if(strcmp(format, "int") == 0) evaluateInt(bits);
	else evaluateFloat(bits);

	return 0;
}
