#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "y86tools.h"
#include "futil.h"

int *registers;
unsigned char *memory;
int OF = 0, ZF = 0, SF = 0;
int count, memorySize;
Status status = AOK;

void push(int val){
        setRegister(ESP, getRegister(ESP) - 4);
        setLong(getRegister(ESP), val);
}

int pop(){
        int val = getLong(getRegister(ESP));
        setRegister(ESP, getRegister(ESP) + 4);
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
	int endInstructionsMemory = -1;
        while( strlen((token = getNextToken(file))) != 0){
//              printf("TOKEN: %s\n", token);
                if(strcmp(token, ".size") == 0){
                        if(setMemorySize(getNextToken(file)) == 1) return -1;
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
                        endInstructionsMemory = (setInstructions(a, b));
                        free(a);
                        free(b);
                }

                free(token);
        }
        return endInstructionsMemory;
}

int setInstructions(char *address, char *instructions){
        int i, addy = strtol(address, NULL, 16);
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
                if( i > 0 && i % 2 == 1) addy ++;                                               // Only increase when currently odd.
        }
        return addy;
}


int setMemorySize(char *size){
        if(size == 0 || strlen(size) == 0) return 1;
        memorySize = strtol(size, NULL, 16) + 1;
        setRegister(ESP, memorySize - 1);
        memory = malloc(sizeof(unsigned char) * memorySize);
        return 0;
}

void createRegisters(int num){
        registers = malloc(sizeof(int) * num);
        for( num = num - 1; num >= 0; num --){
                setRegister(num, 0);
        }
}

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
	char buffer[64];
	char *instrName = getInstructionName(instr->opcode);
	buffer[0] = '\0';
	strcat(buffer, instrName);
	append(buffer, ' ');
	appendArguments(buffer, instr);
	fflush(file);
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
                        append(buffer, ',');
                        append(buffer, ' ');
                        regName = getRegisterName(instr->rB);
                        strcat(buffer, regName);
                        free(regName);
			break;
                case MRMOVL:
                case MOVSBL:
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
                case WRITEL:
                case READB:
                case READL:
                        append(buffer, '$');
                        strcat(buffer, temp);
                        append(buffer, '(');
                        regName = getRegisterName(instr->rA);
                        strcat(buffer, regName);
                        free(regName);
                        append(buffer, ')');
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
		default:
			sprintf(name, "%0x", opcode);
			 break;
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
