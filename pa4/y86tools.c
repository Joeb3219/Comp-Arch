#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "y86emul.h"
#include "y86tools.h"
#include "futil.h"

void printMemory(unsigned char *memory, int size, int chars){
	int i = 0;
	for(i = 0; i < size; i ++){
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

void printInstruction(Instr *instr, FILE *file){
	char buffer[64];// = malloc(64);
	char *instrName = getInstructionName(instr->opcode);
	strcat(buffer, instrName);
	append(buffer, ' ');
	appendArguments(buffer, instr);
	fprintf(file, "%s\n", buffer);
	free(instrName);
}

void appendArguments(char *buffer, Instr *instr){
	char *temp = malloc(12), *regName;
	sprintf(temp, "0x%0X", instr->d);
	switch(instr->opcode){
		case NOP: break;
                case HALT: break;
                case IRMOVL:
			append(buffer, '$');
			strcat(buffer, temp);
			break;
                case MRMOVL:
			append(buffer, '$');
			strcat(buffer, temp);
			append(buffer, '(');
			regName = getRegisterName(instr->rB);
			strcat(buffer, regName);
			free(regName);
			append(buffer, ')');
			append(buffer, ',');
			append(buffer, ' ');
                        regName = getRegisterName(instr->rA);
                        strcat(buffer, regName);
                        free(regName);
			break;
                case RMMOVL:
                        regName = getRegisterName(instr->rA);
                        strcat(buffer, regName);
                        free(regName);
			append(buffer, ',');
                        append(buffer, ' ');
			append(buffer, '$');
                        strcat(buffer, temp);
                        append(buffer, '(');
                        regName = getRegisterName(instr->rB);
                        strcat(buffer, regName);
                        free(regName);
                        append(buffer, ')');
			break;
                case RRMOVL:
                case ADDL:
                case SUBL:
                case MULL:
                case ANDL:
                case XORL:
                case CMPL:
                        regName = getRegisterName(instr->rA);
                        strcat(buffer, regName);
                        free(regName);
			append(buffer, ',');
			append(buffer, ' ');
                        regName = getRegisterName(instr->rB);
                        strcat(buffer, regName);
                        free(regName);
			break;
                case JMP:
                case JGE:
                case JG:
                case JL:
                case JLE:
                case JE:
                case JNE:
                case CALL:
			append(buffer, '$');
			strcat(buffer, temp);
			break;
                case PUSHL:
                case POPL:
                        regName = getRegisterName(instr->rA);
                        strcat(buffer, regName);
                        free(regName);
                        break;
                case RET:
                        break;
                case WRITEB:
                        break;
                case WRITEL:
                        break;
                case READB:
                        break;
                case READL:
                        break;
                case MOVSBL:
                        break;
	}
	if(temp != 0) free(temp);
}

char* getInstructionName(Opcode opcode){
	char *name = malloc(7);
	switch(opcode){
		case NOP: strcpy(name, "NOP"); break;
		case HALT: strcpy(name, "HLT"); break;
		case IRMOVL: strcpy(name, "IRMOVL"); break;
		case MRMOVL: strcpy(name, "MRMVOL"); break;
		case RMMOVL: strcpy(name, "RMMOVL"); break;
		case RRMOVL: strcpy(name, "RRMOVL"); break;
		case ADDL: strcpy(name, "ADDL"); break;
		case SUBL: strcpy(name, "SUBL"); break;
		case MULL: strcpy(name, "MULL"); break;
		case ANDL: strcpy(name, "ANDL"); break;
		case XORL: strcpy(name, "XORL"); break;
		case CMPL: strcpy(name, "CMPL"); break;
		case JMP: strcpy(name, "JMP"); break;
		case JGE: strcpy(name, "JGE"); break;
		case JG: strcpy(name, "JG"); break;
		case JL: strcpy(name, "JL"); break;
		case JLE: strcpy(name, "JLE"); break;
		case JE: strcpy(name, "JE"); break;
		case JNE: strcpy(name, "JNE"); break;
		case PUSHL: strcpy(name, "PUSHL"); break;
		case POPL: strcpy(name, "POPL"); break;
		case CALL: strcpy(name, "CALL"); break;
		case RET: strcpy(name, "RET"); break;
		case WRITEB: strcpy(name, "WRITEB"); break;
		case WRITEL: strcpy(name, "WRITEL"); break;
		case READB: strcpy(name, "READB"); break;
		case READL: strcpy(name, "READL"); break;
		case MOVSBL: strcpy(name, "MOVSBL"); break;
		default: strcpy(name, "err"); break;
	}
	return name;
}

void printRegisters(unsigned int *registers){
	printf("REGISTERS:\n");
	int i;
	for(i = 0; i < 8; i ++){
		printf("[%d]: %d\n", i, registers[i]);
	}
	printf("END REGISTERS\n");
}

char* getRegisterName(int id){
	switch(id){
		case EAX: return strdup("%eax");
		case EBX: return strdup("%ebx");
		case ECX: return strdup("%ecx");
		case EDX: return strdup("%edx");
		case ESP: return strdup("%esp");
		case EBP: return strdup("%ebp");
		case ESI: return strdup("%esi");
		case EDI: return strdup("%edi");
	}
	return strdup("%err");
}
