#include <stdlib.h>
#include <stdio.h>

/*
 * The strategy is to create an array of the first 199 numbers. This array is initialized to -1 by default.
 * Every time fib(n) is computed, we will store arr(n) = fib(n).
 * When we call fib(n), we will see if arr(n) = -1.
 * If it is, then we will compute it by fib(n-1) + fin(n-2), and store the result in arr(n).
 * If it isn't, then we will just set fin(n) = arr(n).
 */

static int num[200];

int dothething(int n){
	if(n >= 200 || n < 0) return 0;
	if(num[n] != -1) return num[n];
	num[n] = dothething(n - 1) + dothething(n - 2);
	return num[n];
}

int main(int argc, char ** argv){
	int i, n;
	for(i = 0; i < 200; i ++){
		num[i] = -1;
	}
	num[0] = num[1] = 1;
	n = atoi(argv[1]);
	n --;
	printf("Value: %d\n", dothething(n));
	return 0;
}
