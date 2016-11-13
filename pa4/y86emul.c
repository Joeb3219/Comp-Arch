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

void setRegister(int id, int val){
	if(id >= 8 || id < 0){
		printf("ERROR: ATTEMPTING TO SET REGISTER OUT OF BOUNDS: %d\n", id);
		exit(1);
	}
	registers[id] = val;
}

unsigned int getRegister(int id){
	if(id >= 8 || id < 0){
                printf("ERROR: ATTEMPTING TO GET REGISTER OUT OF BOUNDS: %d\n", id);
                exit(1);
        }
	return registers[id];
}

void setMemory(int id, unsigned int val){
	if(id >= memorySize || id < 0){
                printf("ERROR: ATTEMPTING TO SET MEMORY OUT OF BOUNDS: %d\n", id);
                exit(1);
	}
	memory[id] = val;
}

unsigned int getMemory(int id){
        if(id >= memorySize || id < 0){
                printf("ERROR: ATTEMPTING TO GET MEMORY OUT OF BOUNDS: %d\n", id);
                exit(1);
        }
	return memory[id];
}

void setLong(int id, unsigned int val){
	if(id >= memorySize - 8 || id < 0){
                printf("ERROR: ATTEMPTING TO SET MEMORY OUT OF BOUNDS: %d\n", id);
                exit(1);
        }

	int i = 0;
	char res[9];
	sprintf(&res[0], "%08x", val);

	for(i = 0; i < 8; i ++){
		setMemory(id + i, res[8 - i - 1]);
	}
}

unsigned int interpretLong(int startingAddy){
        int i = 0, dig;
        char rep[9];
        for(i = 0; i < 8; i ++){
                dig = memory[startingAddy + i/2];
		if(i % 2 == 0) rep[8 - 1 - i] = digToHexChar(dig & 15);
		else rep[7 - i] = digToHexChar((dig & 240) >> 4);
	}
        rep[8] = '\n';
        return strtol(rep, NULL, 16);
}

unsigned int getLong(int id){
	if(id >= memorySize - 8 || id < 0){
                printf("ERROR: ATTEMPTING TO GET MEMORY OUT OF BOUNDS: %d\n", id);
                exit(1);
        }

	return interpretLong(id);
}

int loadMemoryAddy(int registerId, int displacement){
        int val = getRegister(registerId) + displacement;
        return val;
}


void loadArgs(Instr *instr, int addy){
	addy ++; // Move to the first argument if it exists. addy and addy + 1 describe the instruction, not its arguments.
	char rep[9];
	instr->rA = (memory[addy] & 240) >> 4;
	instr->rB = (memory[addy] & 15);
	addy ++;
	if(instr->opcode >= 0x70 && instr->opcode <= 0x80) addy --;
	instr->d = interpretLong(addy);
}

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

Status execute(Instr* instr){
	if(DEBUG >= 2) printInstruction(instr);
	switch(instr->opcode){
		case NOP:
			break;
		case HALT:
			return HLT;
			break;
		case RRMOVL:
			count += 1;
			if(DEBUG >= 1) printf("Putting reg[%d] -> reg[%d]\n", instr->rA, instr->rB);
                        setRegister(instr->rA, getRegister(instr->rB));
			break;
                case IRMOVL:
			count += 5;
			if(DEBUG >= 1) printf("Putting %d -> reg[%d]\n", instr->d, instr->rB);
			setRegister(instr->rB, instr->d);
                        break;
                case RMMOVL:
			count += 5;
			if(DEBUG >= 1) printf("Putting reg[%d] -> memory[%d(%d)]\n", instr->rA, instr->d, instr->rB);
                        setLong(instr->rB + instr->d, instr->rA);
			break;
                case MRMOVL:
			count += 5;
			if(DEBUG >= 1) printf("Putting reg[%d] <- memory[%d(%d)]\n", instr->rA, instr->d, instr->rB);
                        setRegister(instr->rA, getLong(instr->d * instr->rB));
                        break;
                case JMP:
                        break;
                case JLE:
                        break;
                case JL:
                        break;
                case JE:
                        break;
                case JNE:
			if(ZF != 1) count = instr->d;
                        break;
                case JGE:
                        break;
                case JG:
                        break;
                case CALL:
                        break;
                case RET:
                        break;
                case PUSHL:
                        break;
                case POPL:
                        break;
                case READB:
                        break;
                case READL:
                        break;
                case WRITEB:
			count += 5;
			printf("%c", getMemory(instr->d + getRegister(instr->rA)));
			break;
                case WRITEL:
                        break;
                case ADDL:
			count += 1;
			setRegister(instr->rB, getRegister(instr->rA) + getRegister(instr->rB));
                        break;
                case SUBL:
			count += 1;
			setRegister(instr->rB, getRegister(instr->rB) - getRegister(instr->rA));
       			if(getRegister(instr->rB) == 0){
				ZF = 1;
				OF = 0;
				SF = 0;
			}else{
				ZF = 0;
				OF = 0;
				SF = 0;
			}
	                 break;
                case MULL:
			count += 1;
			setRegister(instr->rB, getRegister(instr->rA) * getRegister(instr->rB));
                        break;
                case ANDL:
                        break;
                case XORL:
                        break;
		case CMPL:
			break;
		default:
			break;
	}
	return AOK;
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
	unsigned char val, left, right;
	count = addy;
	if(DEBUG >= 1) printf("Inserting instructions at address %d: %s\n", addy, instructions);
	for(i = 0; i < strlen(instructions); i ++){
		val = hexCharToDig(instructions[i]);
		left = getMemory(addy) & 240;
		right = getMemory(addy) & 15;
		if(i % 2 == 0){
			memory[addy] = (val << 4) | right;
		}else{
			memory[addy] = left | val;
		}
		if( i > 0 && i % 2 == 1) addy ++; 						// Only increase when currently odd.
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

void createRegisters(int num){
	printf("Creating %d registers\n", num);
	registers = malloc(sizeof(unsigned int) * num);
	for( num = num - 1; num >= 0; num --){
		setRegister(num, 0);
	}
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

	createRegisters(8);

	executeProgram();

	closeFile(file);

	return 0;
}
