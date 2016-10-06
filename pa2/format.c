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


void evaluateInt(char *bits){
	printf("%d\n", evaluateBinary(bits, TWO_COMPLEMENT));
}

char* evaluateFloat(char *bits){
	char expBits[9], magBits [25], *buffer = malloc(33);
	int magnitudeDigits[24], magnitude, exponent, sign, i, power = 0;
	double magVal = 0;

	strncpy(expBits, &bits[1], 8);
	expBits[8] = '\0';

	magBits[0] = '1';
	strncpy(&magBits[1], &bits[9], 24);
	magBits[24] = '\0';

	exponent = evaluateBinary(expBits, EXPONENT);
	if(exponent == INF && sign == 1) return "ninf";
	else if(exponent == INF) return "pinf";
	sign = (bits[0] == '1');
	exponent = pow(2, exponent);
	
	for(i = 0; i < (sizeof(magBits) / sizeof(magBits[0])); i ++) magVal += ( (magBits[i] - '0') * pow(2, power--)) * exponent;

	power = 0;
	if(magVal < 0){
		while(magVal < -10){
        	        power --;
                	magVal *= 10.0;
        	}
	}else{
		while(magVal > 10){
			power ++;
			magVal /= 10.0;
		}
	}

	buffer[0] = (sign == 1) ? '-' : ' ';
	sprintf(&buffer[1], "%1.6f", magVal);
	sprintf(&buffer[2 + 6], "e%d\n", power);
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
