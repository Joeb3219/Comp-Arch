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
        int i = 0;
        char rep[9];
        for(i = 0; i < 8; i ++){
                rep[i] = digToHexChar(memory[startingAddy + 7 - i]);
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
	count += 2;
	return val;
}

Instr* decode(int addy){
	Instr *instr = malloc(sizeof(Instr));
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
			count += 10;
			break;
		case 3:
			instr->opcode = IRMOVL;
			loadArgs(instr, addy, 0, 1, 0, 1);
			count += 10;
			break;
		case 4:
			instr->opcode = RMMOVL;
			loadArgs(instr, addy, 1, 1, 1, 0);
			count += 10;
			break;
		case 5:
			instr->opcode = MRMOVL;
			loadArgs(instr, addy, 1, 1, 1, 0);
			count += 10;
			break;
		case 6:
			loadArgs(instr, addy, 1, 1, 0, 0);
			count += 2;
			if(memory[addy + 1] == 0) instr->opcode = ADDL;
			if(memory[addy + 1] == 1) instr->opcode = SUBL;
			if(memory[addy + 1] == 2) instr->opcode = ANDL;
			if(memory[addy + 1] == 3) instr->opcode = XORL;
			if(memory[addy + 1] == 4) instr->opcode = MULL;
			if(memory[addy + 1] == 5) instr->opcode = CMPL;
			break;
		case 7:
			loadArgs(instr, addy, 0, 0, 0, 1);
			count += 8;
			if(memory[addy + 1] == 0) instr->opcode = JMP;
			if(memory[addy + 1] == 1) instr->opcode = JLE;
			if(memory[addy + 1] == 2) instr->opcode = JL;
			if(memory[addy + 1] == 3) instr->opcode = JE;
			if(memory[addy + 1] == 4) instr->opcode = JNE;
			if(memory[addy + 1] == 5) instr->opcode = JGE;
			if(memory[addy + 1] == 6) instr->opcode = JG;
			break;
		case 8:
			count += 8;
			loadArgs(instr, addy, 1, 1, 0, 1);
			instr->opcode = CALL;
			break;
		case 9:
			instr->args = 0;
			instr->opcode = RET;
			break;
		case 10:
			count += 10;
			loadArgs(instr, addy, 1, 0, 0, 0);
			instr->opcode = PUSHL;
			break;
		case 11:
			count += 10;
			loadArgs(instr, addy, 1, 0, 0, 0);
			instr->opcode = POPL;
			break;
		case 12:
			count += 10;
			loadArgs(instr, addy, 1, 0, 1, 0);
			if(memory[addy + 1] == 0) instr->opcode = READB;
			if(memory[addy + 1] == 1) instr->opcode = READL;
			break;
		case 13:
			count += 10;
			loadArgs(instr, addy, 1, 0, 1, 0);
                        if(memory[addy + 1] == 0) instr->opcode = WRITEB;
                        if(memory[addy + 1] == 1) instr->opcode = WRITEL;
			break;
		case 14:
			count += 10;
			loadArgs(instr, addy, 1, 1, 1, 0);
			instr->opcode = MOVSBL;
	}

	return instr;
}

Status execute(Instr* instr){
	printInstruction(instr);
	switch(instr->opcode){
		case NOP:
			break;
		case HALT:
			return HLT;
			break;
		case RRMOVL:
			if(DEBUG >= 1) printf("Putting reg[%d] -> reg[%d]\n", instr->operands[0], instr->operands[1]);
                        setRegister(instr->operands[1], getRegister(instr->operands[0]));
			break;
                case IRMOVL:
			if(DEBUG >= 1) printf("Putting %d -> reg[%d]\n", instr->operands[0], instr->operands[1]); 
			setRegister(instr->operands[1], instr->operands[0]);
                        break;
                case RMMOVL:
			if(DEBUG >= 1) printf("Putting reg[%d] -> memory[%d]\n", instr->operands[0], instr->operands[1]);
                        setLong(instr->operands[1], getRegister(instr->operands[0]));
			break;
                case MRMOVL:
			if(DEBUG >= 1) printf("Putting reg[%d] <- memory[%d]\n", instr->operands[0], instr->operands[1]);
                        setRegister(instr->operands[1], getLong(instr->operands[0]));
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
                        break;
                case WRITEL:
                        break;
                case ADDL:
                        break;
                case SUBL:
                        break;
                case MULL:
                        break;
                case ANDL:
                        break;
                case XORL:
                        break;
		case CMPL:
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
