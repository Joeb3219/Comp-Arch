#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "nCr.h"

int main(int argc, char **argv){
	int n, r = 1, val;
	if(argc != 2){
		printf("Invalid number of arguments: %d, expected 2", argc);
		return 1;
	}
	if(strcmp(argv[1], "-h") == 0){
		printf("Usage: formula <positive integer>");
		return 1;
	}

	n = atoi(argv[1]);
	if(Factorial(n) == 0){
		printf("%d! IS TOO LARGE TO COMPUTE", n);
		return 1;
	}

	printf("1");
	while(r <= n){
		val = nCr(n, r);
		printf(" + %d*x^%d", val, r);
		r ++;
	}

	printf("\n");

	return 0;
}
