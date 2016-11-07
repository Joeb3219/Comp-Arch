#include <stdlib.h>
#include <stdio.h>
#include "mystery.h"

/*
 * The strategy is to create an array of the first 199 numbers. This array is initialized to -1 by default.
 * Every time fib(n) is computed, we will store arr(n) = fib(n).
 * When we call fib(n), we will see if arr(n) = -1.
 * If it is, then we will compute it by fib(n-1) + fin(n-2), and store the result in arr(n).
 * If it isn't, then we will just set fin(n) = arr(n).
 */

static int num[200];

int add(int a, int b){
	return a + b;
}

int dothething(int n){
	if(n >= 200 || n < 0) return 0;				// Check that we're not out of bounds
	if(num[n] != -1) return num[n];				// If we already defined fib(n), return fib(n).
	num[n] = add(dothething(n - 1), dothething(n - 2));	// Otherwise, recursively compute it.
	return num[n];						// Return the val, which is stored for later.
}

int main(int argc, char ** argv){
	int i, n, val;
	for(i = 0; i < 200; i ++){				// Set the values in the array to -1 by default.
		num[i] = -1;
	}
	num[0] = num[1] = 1;					// fib(0) = fib(1) = 1, already defined
	n = atoi(argv[1]);					// A lazy conversion, how wonderful.
	if(n > 200 || n <= 0) val = 0;				// If out of bounds, just return 0 by default.
	else val = dothething(n - 1); 				// We're zero based, so set this back one.
	printf("Value: %d\n", val);
	return 0;
}
