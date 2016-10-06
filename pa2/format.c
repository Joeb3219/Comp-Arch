#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#define TWO_COMPLEMENT 0
#define ONE_COMPLEMENT 1
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
	printf("Float\n");	
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
