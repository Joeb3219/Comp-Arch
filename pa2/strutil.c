#include "strutil.h"

/*
 * Given an integer, i, which is <= 15, will return the hex representation of the number.
 * IE: 0 -> 0, 1 -> 1, .. 10 -> A
 */
char digToChar(int i){
        if(i < 10) return '0' + i;
        return 'A' + (i - 10);
}


/*
 * Adds a character, c, to a string. We assume str is long enough to hold the character plus a null byte.
 */
void addChar(char *str, char c){
        char buffer[2];
        buffer[0] = c;
        buffer[1] = '\0';
        strcat(str, buffer);
}

void intToString(char *buffer, int number){
	if(number == 0){
                addChar(buffer, '0');
                return;
        }

	int numDigits = 0, i = 0;
        char internalBuffer[10];
        internalBuffer[0] = '\0';

	if(number < 0){
                addChar(buffer, '-');
        //        number *= -1;
        }

	while(number != 0){
		numDigits ++;
                addChar(internalBuffer, '0' + ((number % 10) * ((number < 0) ? -1 : 1)));
                number /= 10;
        }

        internalBuffer[numDigits] = '\0';
        for(i = numDigits - 1; i >= 0; i --) addChar(buffer, internalBuffer[i]);
}

void floatToString(char *buffer, float number){
        int rep = 0;
        memcpy(&rep, &number, sizeof(rep));

        int exp = rep >> 23 & 0x000000ff;
        int sign = rep >> 31;
        int mantissa = rep & 0x007fffff;

        if(exp == INF){
                if(mantissa == 0 && sign != 0){
                        strcpy(buffer, "pinf");
                        return;
                }if(mantissa == 0 && sign == 0){
                        strcpy(buffer, "ninf");
                        return;
                }if(mantissa != 0){
                        strcpy(buffer, "NaN");
                        return;
		}
        }

        int power = 0, i = 0;
        if(sign != 0){
                number *= -1;
                addChar(buffer, '-');
        }
        if(number == 0){
                addChar(buffer,'0');
                addChar(buffer,'e');
                addChar(buffer,'0');
                return;
        }
        while(number >= 10.0){
                number /= 10.0;
                power += 1;
        }
        while(number < 1.0){
                number *= 10.0;
                power -= 1;
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

/*
 * Converts a character representation of a digit (ie: ASCII) into an integer.
 * IE: Converts 'B'->11, '0'->0, etc.
 */
int charToDig(char c){
        if(c >= '0' && c <= '9') return c - '0';
        if(c >= 'a' && c <= 'f') return c - 'a' + 10;
        return c - 'A' + 10;
}

/*
 * Copies a uchar array of the indicated size and returns the duplicate.
 */
uchar* copyArray(uchar *arr, int size){
        uchar *res = malloc(sizeof(uchar) * size);
        if(size < 1) return res;
        int i;
        for(i = 0; i < size; i ++) res[i] = arr[i];
        return res;
}
