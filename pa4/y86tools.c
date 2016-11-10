#include <stdlib.h>
#include <stdio.h>

void printMemory(unsigned char *memory, int size, int chars){
	int i = 0;
	for(i; i < size; i ++){
        	if(i % 16 == 0) printf("\n%5d: ", i);
        	if(chars == 0) printf("[%08x]", memory[i]);
		else{
			if(memory[i] != 0) printf("[%c]", memory[i]);
			else printf("[ ]");
		}
	}
	printf("\n");
}
