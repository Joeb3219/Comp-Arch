#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include "y86emul.h"
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

void execute(Instr* instr){
	int a, b, x;
	char buffer[9];
	if(DEBUG >= 2) printInstruction(instr, stdout);
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
                        setLong(getRegister(instr->rB) + instr->d, getRegister(instr->rA));
			break;
                case MRMOVL:
			count += 5;
                        setRegister(instr->rA, getLong(instr->d + getRegister(instr->rB)));
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
			push(count + 4);
			count = instr->d;
                        break;
                case RET:
			count = pop();
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
			setLong(instr->d + getRegister(instr->rA), strtol(buffer, NULL, 10));
			break;
                case READL:
			count += 5;
			ZF = (read(STDIN_FILENO, buffer, 8) == 0);
			buffer[8] = '\n';
			setLong(instr->d + getRegister(instr->rA), strtol(buffer, NULL, 10));
			break;
                case WRITEB:
			count += 5;
			printf("%c", getMemory(instr->d + getRegister(instr->rA)));
			fflush(stdout);
			break;
                case WRITEL:
			count += 5;
			a = getLong(getRegister(instr->rA) + instr->d);
			fflush(stdout);
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
			a = getRegister(instr->rA);
			b = getRegister(instr->rB);
			x = b - a;
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

	createRegisters(8);

	if(loadProgramIntoMemory(file) == 1){
		printf("Error processing file.\n");
		return 1;
	}

	executeProgram();

	closeFile(file);

	return 0;
}
