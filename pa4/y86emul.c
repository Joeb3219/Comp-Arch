#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include "y86emul.h"
#include "futil.h"
#include "y86tools.h"

static int *registers;
static unsigned char *memory;
static int OF = 0, ZF = 0, SF = 0;
static int count = 0, memorySize;
static Status status = AOK;
static FILE *instrOutput;

void setRegister(int id, int val){
	if(id >= 8 || id < 0){
		printf("ERROR: ATTEMPTING TO SET REGISTER OUT OF BOUNDS: %d\n", id);
		status = ADR;
		return;
	}
	registers[id] = val;
}

int getRegister(int id){
	if(id >= 8 || id < 0){
                printf("ERROR: ATTEMPTING TO GET REGISTER OUT OF BOUNDS: %d\n", id);
		status = ADR;
		return -1;
        }
	return registers[id];
}

void setMemory(int id, int val){
	if(id >= memorySize || id < 0){
                printf("ERROR: ATTEMPTING TO SET MEMORY OUT OF BOUNDS: %d\n", id);
		status = ADR;
		return;
	}
	memory[id] = val;
}

int getMemory(int id){
        if(id >= memorySize || id < 0){
                printf("ERROR: ATTEMPTING TO GET MEMORY OUT OF BOUNDS: %d\n", id);
		status = ADR;
		return -1;
        }
	return memory[id];
}

void setLong(int id, int val){
	if(id >= memorySize - 4 || id < 0){
                printf("ERROR: ATTEMPTING TO SET MEMORY OUT OF BOUNDS: %d\n", id);
		status = ADR;
		return;
        }

	int i = 0, j = id;
	char res[9];
	sprintf(&res[0], "%08x", val);
	for(i = 7; i >= 1; i -= 2){
		val = (hexCharToDig(res[i])) | (hexCharToDig(res[i - 1]) << 4);
		setMemory(j, val);
		j ++;
	}
}

int interpretLong(int startingAddy){
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

int getLong(int id){
	if(id >= memorySize - 4 || id < 0){
                printf("ERROR: ATTEMPTING TO GET MEMORY OUT OF BOUNDS: %d\n", id);
		status = ADR;
		return -1;
        }

	return interpretLong(id);
}

int loadMemoryAddy(int registerId, int displacement){
        int val = getRegister(registerId) + displacement;
        return val;
}


void loadArgs(Instr *instr, int addy){
	addy ++; // Move to the first argument if it exists. addy and addy + 1 describe the instruction, not its arguments.
	instr->rA = (memory[addy] & 240) >> 4;
	instr->rB = (memory[addy] & 15);
	addy ++;
	if(instr->opcode >= 0x70 && instr->opcode <= 0x80) addy --;
	instr->d = interpretLong(addy);
}

int fetch(){
//	printf("[C:0x%0X,%d,%0x]\n", count, count - 0x100, memory[count]);
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

void push(int val){
	setRegister(ESP, getRegister(ESP) - 4);
	setLong(getRegister(ESP), val);
}

int pop(){
	int val = getLong(getRegister(ESP));
	setRegister(ESP, getRegister(ESP) + 4);
	return val;
}

void execute(Instr* instr){
	int a, b, x;
	char buffer[9];
	if(DEBUG >= 2 || DISASSEMBLE == 1) printInstruction(instr, instrOutput);
	switch(instr->opcode){
		case NOP:
			break;
		case HALT:
			status = HLT;
			break;
		case RRMOVL:
			count += 1;
                        setRegister(instr->rB, getRegister(instr->rA));
			break;
                case IRMOVL:
			count += 5;
			setRegister(instr->rB, instr->d);
                        break;
                case RMMOVL:
			count += 5;
                        setLong(instr->rB + instr->d, instr->rA);
			break;
                case MRMOVL:
			count += 5;
                        setRegister(instr->rA, getLong(instr->d * instr->rB));
                        break;
                case JMP:
			count = instr->d;
			break;
                case JLE:
			if(ZF == 1 || (SF != OF)) count = instr->d;
                        else count += 4;
			break;
                case JL:
			if(ZF == 0 && SF != OF) count = instr->d;
                        else count += 4;
                        break;
                case JE:
			if(ZF == 1) count = instr->d;
                        else count += 4;
                        break;
                case JNE:
			if(ZF != 1) count = instr->d;
                        else count += 4;
                        break;
                case JGE:
			if(ZF == 1 || SF == OF) count = instr->d;
                        else count += 4;
                        break;
                case JG:
			if(ZF == 0 && SF == OF) count = instr->d;
                        else count += 4;
                        break;
                case CALL:
			printf("Pushing %d and jumping to %d\n", count + 4, instr->d);
			push(count + 4);
			count = instr->d;
                        break;
                case RET:
			count = pop();
                        printf("popping %d\n", count);
			break;
                case PUSHL:
			count ++;
                        push(getRegister(instr->rA));
			break;
                case POPL:
			count ++;
			setRegister(instr->rA, pop());
                        break;
                case READB:
			count += 5;
                        ZF = (0 == read(STDIN_FILENO, buffer, 2));
                        buffer[2] = buffer[3] = buffer[4] = buffer[5] = buffer[6] = buffer[7] = 0;
			buffer[8] = '\n';
			setLong(instr->d + instr->rA, strtol(buffer, NULL, 16));
			break;
                case READL:
			count += 5;
			ZF = (read(STDIN_FILENO, buffer, 8) == 0);
			buffer[8] = '\n';
			setLong(instr->d + instr->rA, strtol(buffer, NULL, 16));
                        break;
                case WRITEB:
			count += 5;
			printf("%c", getMemory(instr->d + getRegister(instr->rA)));
			break;
                case WRITEL:
			count += 5;
			a = getLong(getRegister(instr->rA) + instr->d);
			printf("%d\n", a);
                        break;
                case ADDL:
			count += 1;
			a = getRegister(instr->rA);
			b = getRegister(instr->rB);
			x = a + b;
			OF = ((a > 0 && b > INT_MAX - a) || (a < 0 && b < INT_MIN - a));
			SF = (x < 0);
			ZF = (x == 0);
			setRegister(instr->rB, x);
                        break;
                case SUBL:
			count += 1;
			a = -getRegister(instr->rA);
			b = getRegister(instr->rB);
			x = b + a;
			OF = ((a > 0 && b > INT_MAX - a) || (-a < 0 && b < INT_MIN - a));
			SF = (x < 0);
			ZF = (x == 0);
			setRegister(instr->rB, x);
	                break;
                case MULL:
			count += 1;
			a = getRegister(instr->rA);
                        b = getRegister(instr->rB);
                        x = a * b;
			ZF = (x == 0);
			OF = !(a != 0 && x / a == b);
			SF = x < 0;
			setRegister(instr->rB, x);
                        break;
                case ANDL:
			count ++;
			a = getRegister(instr->rA);
			b = getRegister(instr->rB);
			x = a&b;
			OF = 0;
			SF = x<0;
			ZF = x==0;
			setRegister(instr->rB, x);
                        break;
                case XORL:
			count ++;
			a = getRegister(instr->rA);
			b = getRegister(instr->rB);
			x = a | b;
			OF = 0;
			ZF = x==0;
			SF = x<0;
			setRegister(instr->rB, x);
                        break;
		case CMPL:
			count ++;
			a = getRegister(instr->rA);
			b = getRegister(instr->rB);
			ZF = (a == b);
			SF = (b+a)<0;
			OF = ((a > 0 && b > INT_MAX - a) || (a < 0 && b < INT_MIN - a));
			break;
		case MOVSBL:
			count += 5;
			setRegister(instr->rA, getMemory(getRegister(instr->rB) + instr->d));
			break;
		default:
			status = INS;
			break;
	}
}

int setMemorySize(char *size){
	if(size == 0 || strlen(size) == 0) return 1;
	memorySize = strtol(size, NULL, 16) + 1;
	setRegister(ESP, memorySize - 1);
	if(DEBUG >= 3) printf("Allocated %d memory blocks for program execution.\n", memorySize);
	memory = malloc(sizeof(unsigned char) * memorySize);
	return 0;
}

int setInstructions(char *address, char *instructions){
	int i, addy = strtol(address, NULL, 16);
	printf("STarting addy: %d\n", addy);
	unsigned char val, left, right;
	count = addy;
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
	return 0;
}

void loadByteIntoMemory(char *address, char *value){
	int addy = strtol(address, NULL, 16);
	char val = (char) strtol(value, NULL, 16);
	memory[addy] = val;
}

void loadLongIntoMemory(char *address, char *value){
	int addy = strtol(address, NULL, 16);
        int val = (int) strtol(value, NULL, 16);
        memory[addy] = val;
}

void loadStringIntoMemory(char *address, char *string){
	int addy = strtol(address, NULL, 16);
	int i = 0;
	for(i = 0; i < strlen(string); i ++){
		memory[i + addy] = string[i];
	}
	memory[i + addy + 1] = '\n';
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
	registers = malloc(sizeof(int) * num);
	for( num = num - 1; num >= 0; num --){
		setRegister(num, 0);
	}
}

void executeProgram(){
	do{
		Instr *instr = decode(fetch());
		execute(instr);
		free(instr);
	}while(status == AOK);
	if(status != HLT) printf("Encountered a non-halt status condition: %d\n", status);
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

	if(DISASSEMBLE == 1) instrOutput = getFileOrCreate(strcat(argv[1], "_x86"));
	else instrOutput = stdout;

	createRegisters(8);

	if(loadProgramIntoMemory(file) == 1){
		printf("Error processing file.\n");
		return 1;
	}

	//printMemory(memory, memorySize, 1);

	executeProgram();

	closeFile(file);

	return 0;
}
