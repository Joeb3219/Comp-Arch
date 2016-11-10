#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "y86emul.h"
#include "futil.h"
#include "y86tools.h"

static unsigned int *registers;
static unsigned char *memory;
static int OF = 0, ZF = 0, SF = 0;
static unsigned int count = 0, memorySize;

int interpretLong(int startingAddy){
	int i = 0;
	char rep[5];// = {memory[startingAddy], memory[startingAddy + 1], memory[startingAddy + 2], memory[startingAddy + 3], '\n'};
	for(i = 0; i < 4; i ++){
		rep[i] = digToHexChar(memory[i + startingAddy]);
	}
	rep[4] = '\n';

	return strtol(rep, NULL, 16);
}

int loadMemoryAddy(int registerId, int displacement){
        int val = registers[registerId] + displacement;
        return val;
}


void loadArgs(Instr *instr, int addy, int rA, int rB, int d, int v){
	addy += 2; // Move to the first argument if it exists. addy and addy + 1 describe the instruction, not its arguments.

	if(rA && rB && !d && !v){
		instr->operands = malloc(sizeof(unsigned int) * 2);
		instr->args = 2;
		instr->operands[0] = memory[addy];
		instr->operands[1] = memory[addy + 1];
	}else if(!rA && rB && v && !d){
		addy ++;
		instr->operands = malloc(sizeof(unsigned int) * 2);
                instr->args = 2;
		instr->operands[0] = interpretLong(addy + 1);
		instr->operands[1] = memory[addy];
	}else if(rA && rB && d && !v){
		instr->operands = malloc(sizeof(unsigned int) * 2);
                instr->args = 2;
		if(instr->opcode == RMMOVL){
			instr->operands[0] = memory[addy];
			instr->operands[1] = loadMemoryAddy(memory[addy + 1], interpretLong(memory[addy + 2]));
		}else{
			instr->operands[1] = memory[addy];
                        instr->operands[0] = loadMemoryAddy(memory[addy + 1], interpretLong(memory[addy + 2]));
		}
	}else if(!rA && !rB && d && !v){
		instr->operands = malloc(sizeof(unsigned int) * 1);
                instr->args = 1;
		instr->operands[0] = memory[addy];
	}else if(rA && !rB && !d && !v){
		instr->operands = malloc(sizeof(unsigned int) * 1);
                instr->args = 1;
		instr->operands[0] = memory[addy];
	}
}

int fetch(){
	int val = count;
	count ++;
	return val;
}

Instr* decode(int addy){
	Instr *instr = malloc(sizeof(Instr));
	if(DEBUG >= 1) printf("Current instr @ %d: %d\n", addy, memory[addy]);
	switch(memory[addy]){
		case 0:
			instr->opcode = NOP;
			instr->args = 0;
			break;
		case 1:
			instr->opcode = HALT;
                        instr->args = 0;
			break;
		case 2:
			instr->opcode = RRMOVL;
			loadArgs(instr, addy, 1, 1, 0, 0);
			break;
		case 3:
			instr->opcode = IRMOVL;
			loadArgs(instr, addy, 0, 1, 0, 1);
			break;
		case 4:
			instr->opcode = RMMOVL;
			loadArgs(instr, addy, 1, 1, 1, 0);
			break;
		case 5:
			instr->opcode = MRMOVL;
			loadArgs(instr, addy, 1, 1, 1, 0);
			break;
		case 6:
			loadArgs(instr, addy, 1, 1, 0, 0);
			if(memory[addy + 1] == 0) instr->opcode = ADDL;
			if(memory[addy + 1] == 1) instr->opcode = SUBL;
			if(memory[addy + 1] == 2) instr->opcode = ANDL;
			if(memory[addy + 1] == 3) instr->opcode = XORL;
			if(memory[addy + 1] == 4) instr->opcode = MULL;
			if(memory[addy + 1] == 5) instr->opcode = CMPL;
			break;
		case 7:
			loadArgs(instr, addy, 0, 0, 0, 1);
			if(memory[addy + 1] == 0) instr->opcode = JMP;
			if(memory[addy + 1] == 1) instr->opcode = JLE;
			if(memory[addy + 1] == 2) instr->opcode = JL;
			if(memory[addy + 1] == 3) instr->opcode = JE;
			if(memory[addy + 1] == 4) instr->opcode = JNE;
			if(memory[addy + 1] == 5) instr->opcode = JGE;
			if(memory[addy + 1] == 6) instr->opcode = JG;
			break;
		case 8:
			loadArgs(instr, addy, 1, 1, 0, 1);
			instr->opcode = CALL;
			break;
		case 9:
			instr->args = 0;
			instr->opcode = RET;
			break;
		case 10:
			loadArgs(instr, addy, 1, 0, 0, 0);
			instr->opcode = PUSHL;
			break;
		case 11:
			loadArgs(instr, addy, 1, 0, 0, 0);
			instr->opcode = POPL;
			break;
		case 12:
			loadArgs(instr, addy, 1, 0, 1, 0);
			if(memory[addy + 1] == 0) instr->opcode = READB;
			if(memory[addy + 1] == 1) instr->opcode = READL;
			break;
		case 13:
			loadArgs(instr, addy, 1, 0, 1, 0);
                        if(memory[addy + 1] == 0) instr->opcode = WRITEB;
                        if(memory[addy + 1] == 1) instr->opcode = WRITEL;
			break;
		case 14:
			loadArgs(instr, addy, 1, 1, 1, 0);
			instr->opcode = MOVSBL;
	}
	return instr;
}

Status execute(Instr* instr){
	printInstruction(instr);
	return HLT;
}

int setMemorySize(char *size){
	if(size == 0 || strlen(size) == 0) return 1;
	memorySize = strtol(size, NULL, 16);
	if(DEBUG >= 1) printf("Allocated %d memory blocks for program execution.\n", memorySize);
	memory = malloc(sizeof(unsigned char) * memorySize);
	return 0;
}

int setInstructions(char *address, char *instructions){
	int i, addy = strtol(address, NULL, 16);
	count = addy;
	if(DEBUG >= 1) printf("Inserting instructions at address %d: %s\n", addy, instructions);
	for(i = 0; i < strlen(instructions); i ++){
		memory[addy] = hexCharToDig(instructions[i]);
		addy ++;
	}
	if(DEBUG >= 2) printMemory(memory, memorySize, 1);
	return 0;
}

int loadByteIntoMemory(char *address, char *value){
	int addy = strtol(address, NULL, 16);
	char val = (char) strtol(value, NULL, 16);
	if(DEBUG >= 2) printf("Setting memory[%d] to %d\n", addy, val);
	memory[addy] = val;
}

int loadLongIntoMemory(char *address, char *value){
	int addy = strtol(address, NULL, 16);
        int val = (int) strtol(value, NULL, 16);
        if(DEBUG >= 2) printf("Setting memory[%d] to %d\n", addy, val);
        memory[addy] = val;
}

int loadStringIntoMemory(char *address, char *string){
	int addy = strtol(address, NULL, 16);
	int i = 0;
	for(i = 0; i < strlen(string); i ++){
		memory[i + addy] = string[i];
	}
}

int loadProgramIntoMemory(FILE *file){
	char *token, *a, *b;
	while( strlen((token = getNextToken(file))) != 0){
//		printf("TOKEN: %s\n", token);
		if(strcmp(token, ".size") == 0){
			if(setMemorySize(getNextToken(file)) == 1) return 1;
		}else if(strcmp(token, ".string") == 0){
			a = getNextToken(file);
			b = getNextToken(file);
			loadStringIntoMemory(a, b);
			free(a);
			free(b);
		}else if(strcmp(token, ".long") == 0){
			a = getNextToken(file);
			b = getNextToken(file);
                	loadLongIntoMemory(a, b);
			free(a);
			free(b);
		}else if(strcmp(token, ".byte") == 0){
			a = getNextToken(file);
                        b = getNextToken(file);
                        loadByteIntoMemory(a, b);
                        free(a);
                        free(b);
                }else if(strcmp(token, ".bss") == 0){

                }else if(strcmp(token, ".text") == 0){
			a = getNextToken(file);
			b = getNextToken(file);
			if(setInstructions(a, b) != 0) return 1;
                	free(a);
			free(b);
		}

		free(token);
	}
	return 0;
}

int executeProgram(){
	Status status = AOK;
	do{
		Instr *instr = decode(fetch());
		status = execute(instr);
		free(instr);
	}while(status == AOK);
}

int main(int argc, char **argv){
	FILE *file;

	if(argc != 2){
		printf("Unexpected input count: %d. Use -h for help\n", argc);
		return 1;
	}
	if(strcmp(argv[1], "-h") == 0){
		printf("USAGE: ./y86emul <filename>\n");
		return 1;
	}

	file = getFile(argv[1]);
	if(file == 0){
		printf("File not found: %s\n", argv[1]);
		return 1;
	}

	if(loadProgramIntoMemory(file) == 1){
		printf("Error processing file.\n");
		return 1;
	}

	if(DEBUG >= 2) printMemory(memory, memorySize, 1);

	executeProgram();

	registers = malloc(sizeof(unsigned int) * 8);

	closeFile(file);

	return 0;
}
