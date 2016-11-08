#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "futil.h"

typedef enum opcodes{
	BYTE = -0x1, SIZE = -0x2, TEXT = -0x3, STRING = -0x4, LONG = -0x5, BSS = -0x6,
	NOP = 0x0, HALT = 0x1, RRMOVL = 0x2, IRMOVL = 0x3, RMMOVL = 0x4, MRMOVL = 0x5,
	JMP = 0x6, JLE = 0x7, JL = 0x8, JE = 0x9, JNE = 0xA,
	CALL = 0xB, RET = 0xC, PUSHL = 0xD, POPL = 0xE,
	READB = 0xF, READL = 0x10, WRITEB = 0x11, WRITEL = 0x12,
	MOVSBL = 0x13
} Opcode;

typedef enum status{
	AOK = 0, HLT = 1, ADR = 2, INS = 3
} Status;

typedef struct instr{
	Opcode opcode;
	int *operands;
} Instr;

static unsigned int *registers, *memory;
static int OF = 0, ZF = 0, SF = 0;
static unsigned int count = 0;

int fetch(){
	return 0;
}

int decode(){
	return 0;
}

int execute(){
	return 0;
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

	registers = malloc(sizeof(unsigned int) * 8);

	printf("TOKEN: %s\n", getNextToken(file));
	printf("TOKEN: %s\n", getNextToken(file));
	printf("TOKEN: %s\n", getNextToken(file));
	printf("TOKEN: %s\n", getNextToken(file));
	printf("TOKEN: %s\n", getNextToken(file));
	printf("TOKEN: %s\n", getNextToken(file));

	closeFile(file);

	return 0;
}
