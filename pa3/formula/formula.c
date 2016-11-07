#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "nCr.h"
#include "formula.h"

/*
 * Checks if a string is numeric.
 * Returns 0 if false, or 1 if true.
 */
int isNumeric(char *str){
	int i = 0, n = strlen(str);
	for(i = 0; i < n; i ++){
		if(str[i] < '0' || str[i] > '9') return 0;
	}
	return 1;
}

int main(int argc, char **argv){
	int n, r = 1, val;
	struct timeval start, end;

	gettimeofday(&start, NULL);

	if(argc != 2){
		printf("Invalid number of arguments: %d, expected 2\n", argc);
		return 1;
	}
	if(strcmp(argv[1], "-h") == 0){
		printf("Usage: formula <positive integer>\n");
		return 1;
	}

	if(isNumeric(argv[1]) == 0){
		printf("Illegal value for argument 1: %s, expected a positive integer.\n", argv[1]);
		return 1;
	}

	n = atoi(argv[1]);
	if(Factorial(n) == 0){
		printf("%d! IS TOO LARGE TO COMPUTE\n", n);
		return 1;
	}
	printf("1");
	while(r <= n){
		val = nCr(n, r);
		printf(" + %d*x^%d", val, r);
		r ++;
	}

	printf("\n");

	gettimeofday(&end, NULL);
	printf("Program executed in %ld ms\n", ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)));

	return 0;
}
