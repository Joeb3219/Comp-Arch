#include <stdlib.h>
#include <stdio.h>
#include "y86emul.h"

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

int hexCharToDig(char c){
	if(c >= '0' && c <= '9') return c - '0';
	return c - 'a' + 10;
}

char digToHexChar(unsigned char d){
	if(d < 10) return d + '0';
	return d + 'A' - 10;
}

void printInstruction(Instr *instr){
	printf("INSTR: %0X [", instr->opcode);
	printf("%0X, %0X, %08X", instr->rA, instr->rB, instr->d);
	printf("]\n");
}

void printRegisters(unsigned int *registers){
	printf("REGISTERS:\n");
	int i;
	for(i = 0; i < 8; i ++){
		printf("[%d]: %d\n", i, registers[i]);
	}
	printf("END REGISTERS\n");
}
