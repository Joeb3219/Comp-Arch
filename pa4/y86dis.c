#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "futil.h"
#include "y86tools.h"


int fetch(){
	int val = count;
	count += 1;
	return val;
}

Instr* decode(int addy){
	Instr *instr = malloc(sizeof(Instr));
	instr->opcode = memory[addy];
	loadArgs(instr, addy);
	return instr;
}

void execute(Instr* instr, FILE *output){
	printInstruction(instr, stdout);
	printInstruction(instr, output);
	switch(instr->opcode){
		case NOP:
			break;
		case HALT:
			status = HLT;
			break;
		case RRMOVL:
			count += 1;
			break;
                case IRMOVL:
                case RMMOVL:
                case MRMOVL:
			count += 5;
                        break;
                case JMP:
                case JLE:
                case JL:
                case JE:
                case JNE:
                case JGE:
                case JG:
                case CALL:
			count += 4;
                        break;
                case RET:
			break;
                case PUSHL:
                case POPL:
			count ++;
                        break;
                case READB:
                case READL:
                case WRITEB:
                case WRITEL:
			count += 5;
                        break;
                case ADDL:
                case SUBL:
                case MULL:
                case ANDL:
                case XORL:
                case CMPL:
			count += 1;
                        break;
		case MOVSBL:
			count += 5;
			break;
		default:
			status = INS;
			break;
	}
}



int main(int argc, char **argv){
	if(argc != 2 || strcmp(argv[1], "-h") == 0){
		printf("USAGE: y86dis <filename>\n");
		return 1;
	}

	FILE *file = getFile(argv[1]), *output = getFileOrCreate(strcat(argv[1], "_y86"));
	int endMemoryInstruction = 0;

	if(file == 0){
		printf("File not found: %s\n", argv[1]);
		return 1;
	}


	createRegisters(8);

        if((endMemoryInstruction = loadProgramIntoMemory(file)) == -1){
                printf("Error processing file.\n");
                return 1;
        }

	while(count != endMemoryInstruction){
		execute(decode(fetch()), output);
	}

	closeFile(output);
	closeFile(file);

	printf("Generated a file with all instructions: %s\n", argv[1]);

	return 0;
}
