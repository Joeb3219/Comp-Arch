#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "y86tools.h"
#include "futil.h"

// This set of functions and variables are included everywhere that y86 is run.
// These variables are global, which is sad, but makes it easier to reduce the amount of code redundancy.
// Thus, we only have to define fetch/decode/execute instructions within calling files.
// This file will define everything pertaining to reading in a program, manipulating memory (read/set byte/long), etc.
// See y86tools.h for defines, typedefs, and structs.

int registers[8];		// Our 8 registers.
unsigned char *memory;		// Memory, which is to be created later.
int OF = 0, ZF = 0, SF = 0;	// Status flags.
int count, memorySize;		// Current program count and memory size, to be referenced externally & internally.
Status status = AOK;		// The current status of the machine.

void push(int val){
        setRegister(ESP, getRegister(ESP) - 4);
        setLong(getRegister(ESP), val);
}

int pop(){
        int val = getLong(getRegister(ESP));
        setRegister(ESP, getRegister(ESP) + 4);
        return val;
}

// Loads the arguments of an isntruction.
// Sets rA, rB, and d regardless of what instruction it is.
// The execute phase is left to actually determine what to do with this data.
void loadArgs(Instr *instr, int addy){
        addy ++; // Move to the first argument if it exists. addy and addy + 1 describe the instruction, not its arguments.
        instr->rA = (getMemory(addy) & 240) >> 4;
        instr->rB = (getMemory(addy) & 15);
        addy ++;
        if(instr->opcode >= 0x70 && instr->opcode <= 0x80) addy --;
        instr->d = interpretLong(addy);
}

void setRegister(int id, int val){
        if(id >= 8 || id < 0){
                printf("ERROR: ATTEMPTING TO SET REGISTER OUT OF BOUNDS: %d\n", id);
                status = ADR;
		exit(1);
		return;
        }
        registers[id] = val;
}

int getRegister(int id){
        if(id >= 8 || id < 0){
                printf("ERROR: ATTEMPTING TO GET REGISTER OUT OF BOUNDS: %d\n", id);
                status = ADR;
                exit(1);
		return -1;
        }
        return registers[id];
}

void setMemory(int id, int val){
        if(id >= memorySize || id < 0){
                printf("ERROR: ATTEMPTING TO SET MEMORY OUT OF BOUNDS: 0x%0X\n", id);
                status = ADR;
		exit(1);
		return;
        }
        memory[id] = val;
}

int getMemory(int id){
        if(id >= memorySize || id < 0){
                printf("ERROR: ATTEMPTING TO GET MEMORY OUT OF BOUNDS: 0x%0X\n", id);
		status = ADR;
		exit(1);
                return -1;
        }
        return memory[id];
}

// Converts an int into 4 bytes and sets the respective memory values of addy, addy + 1, addy + 2, and addy + 3 to these bytes, in little endian.
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

// Reads the long starting at an address and spanning addy, addy + 1, addy + 2, addy + 3, in little endian order.
int interpretLong(int startingAddy){
        int i = 0, dig;
        char rep[9];
        for(i = 0; i < 8; i ++){
                dig = getMemory(startingAddy + i/2);
                if(i % 2 == 0) rep[8 - 1 - i] = digToHexChar(dig & 15);
                else rep[7 - i] = digToHexChar((dig & 240) >> 4);
        }
        rep[8] = '\0';
        return strtol(rep, NULL, 16);
}

// Fetches a long at a given memory address.
int getLong(int id){
        if(id >= memorySize - 4 || id < 0){
                printf("ERROR: ATTEMPTING TO GET MEMORY OUT OF BOUNDS: %d\n", id);
                status = ADR;
                return -1;
        }

        return interpretLong(id);
}


// Sets an address to a value, both of which are character arrays.
void loadByteIntoMemory(char *address, char *value){
        int addy = strtol(address, NULL, 16);
        char val = (char) strtol(value, NULL, 16);
        setMemory(addy, val);
}

// Sets an addy to a value, both of which are character arrays.
void loadLongIntoMemory(char *address, char *value){
        int addy = strtol(address, NULL, 16);
        int val = (int) strtol(value, NULL, 16);
        setLong(addy, val);
}

// Loads a string into memory, where the address is a character array.
void loadStringIntoMemory(char *address, char *string){
        int addy = strtol(address, NULL, 16);
        int i = 0;
        for(i = 0; i < strlen(string); i ++){
                setMemory(i + addy, string[i]);
        }
        setMemory(i + addy + 1, '\n');
}

// Creates the memory image, based on whatever .size directive exists in file.
// If .size isn't present, it will return -1 and thus program execution stops.
int findAndSetMemorySize(FILE *file){
	char *token, *a;
	while(strlen((token = getNextToken(file))) != 0){
		if(strcmp(token, ".size") != 0) continue;
		a = getNextToken(file);
		if(setMemorySize(a) == 1){
			free(a);
			return -1;
		}
		free(a);
		return 0;
	}
	return -1;
}

// Loads a program into memory.
// This process is more or less: make a memory image (if no .size, die).
// Then we can go through and load the directives in any order we want, regardless of appearance.
int loadProgramIntoMemory(FILE *file){
        if(file == 0) return -1;
	char *token, *a, *b;
	int endInstructionsMemory = -1;
	if(findAndSetMemorySize(file) == -1) return -1;
	rewind(file);
	while( strlen((token = getNextToken(file))) != 0){
                if(strcmp(token, ".size") == 0) continue;
		else if(strcmp(token, ".string") == 0){
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
	free(token);
        return endInstructionsMemory;
}

// Sets the memory beginning at addy to the instructions, both of which are character arrays.
// We store instructions[0] at addy, instructions[1] at addy, instruction[2] at addy + 1, instruction[3] at addy + 1, etc.
// Thus, we store two instructions per byte.
int setInstructions(char *address, char *instructions){
        int i, addy = strtol(address, NULL, 16);
        unsigned char val, left, right;
        count = addy;
        for(i = 0; i < strlen(instructions); i ++){
                val = hexCharToDig(instructions[i]);
                left = getMemory(addy) & 240;
                right = getMemory(addy) & 15;
                if(i % 2 == 0){
                        setMemory(addy, (val << 4) | right);
                }else{
                        setMemory(addy, left | val);
                }
                if( i > 0 && i % 2 == 1) addy ++;                                               // Only increase when currently odd.
        }
        return addy;
}


int setMemorySize(char *size){
        if(size == 0 || strlen(size) == 0) return 1;
        memorySize = strtol(size, NULL, 16) + 1;
        setRegister(ESP, memorySize - 1);
        memory = calloc(memorySize, sizeof(unsigned char));
	return 0;
}

void printMemory(unsigned char *memory, int size, int chars){
	int i = 0;
	for(i = 0; i < size; i ++){
        	if(i % 16 == 0) printf("\n%5d: ", i);
        	if(chars == 0) printf("[%08x]", getMemory(i));
		else{
			if(getMemory(i) != 0) printf("[%c]", getMemory(i));
			else printf("[ ]");
		}
	}
	printf("\n");
}

int hexCharToDig(char c){
	if(c >= '0' && c <= '9') return c - '0';
	if(c >= 'a' && c <= 'f') return c - 'a' + 10;
	return c - 'A' + 10;
}

char digToHexChar(unsigned char d){
	if(d < 10) return d + '0';
	return d + 'A' - 10;
}


// Will print the instruction to an indicated file, in mnemonic form.
// Converts the opcode to a string and then formats the arguments per the instruction's specifications.
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

// Adds the arugments to a string describing an indicated instruction.
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

Opcode getInstructionOpcode(char* str){
	if(strcmp(str, "nop") == 0) return NOP;
	else if(strcmp(str, "hlt") == 0) return HALT;
	else if(strcmp(str, "irmovl") == 0) return IRMOVL;
	else if(strcmp(str, "mrmovl") == 0) return MRMOVL;
	else if(strcmp(str, "rmmovl") == 0) return RMMOVL;
	else if(strcmp(str, "rrmovl") == 0) return RRMOVL;
	else if(strcmp(str, "addl") == 0) return ADDL;
	else if(strcmp(str, "subl") == 0) return SUBL;
	else if(strcmp(str, "mull") == 0) return MULL;
	else if(strcmp(str, "andl") == 0) return ANDL;
	else if(strcmp(str, "xorl") == 0) return XORL;
	else if(strcmp(str, "cmpl") == 0) return CMPL;
	else if(strcmp(str, "jmp") == 0) return JMP;
	else if(strcmp(str, "jge") == 0) return JGE;
	else if(strcmp(str, "jg") == 0) return JG;
	else if(strcmp(str, "jl") == 0) return JL;
	else if(strcmp(str, "jle") == 0) return JLE;
	else if(strcmp(str, "je") == 0) return JE;
	else if(strcmp(str, "jne") == 0) return JNE;
	else if(strcmp(str, "pushl") == 0) return PUSHL;
	else if(strcmp(str, "popl") == 0) return POPL;
	else if(strcmp(str, "call") == 0) return CALL;
	else if(strcmp(str, "ret") == 0) return RET;
	else if(strcmp(str, "writeb") == 0) return WRITEB;
	else if(strcmp(str, "writel") == 0) return WRITEL;
	else if(strcmp(str, "readb") == 0) return READB;
	else if(strcmp(str, "readl") == 0) return READL;
	else if(strcmp(str, "movsbl") == 0) return MOVSBL;
	return NOP;
}

char* getInstructionName(Opcode opcode){
	char *name = malloc(7);
	switch(opcode){
		case NOP: strcpy(name, "nop"); break;
		case HALT: strcpy(name, "hlt"); break;
		case IRMOVL: strcpy(name, "irmovl"); break;
		case MRMOVL: strcpy(name, "mrmovl"); break;
		case RMMOVL: strcpy(name, "rmmovl"); break;
		case RRMOVL: strcpy(name, "rrmovl"); break;
		case ADDL: strcpy(name, "addl"); break;
		case SUBL: strcpy(name, "subl"); break;
		case MULL: strcpy(name, "mull"); break;
		case ANDL: strcpy(name, "andl"); break;
		case XORL: strcpy(name, "xorl"); break;
		case CMPL: strcpy(name, "cmpl"); break;
		case JMP: strcpy(name, "jmp"); break;
		case JGE: strcpy(name, "jge"); break;
		case JG: strcpy(name, "jg"); break;
		case JL: strcpy(name, "jl"); break;
		case JLE: strcpy(name, "jle"); break;
		case JE: strcpy(name, "je"); break;
		case JNE: strcpy(name, "jne"); break;
		case PUSHL: strcpy(name, "pushl"); break;
		case POPL: strcpy(name, "popl"); break;
		case CALL: strcpy(name, "call"); break;
		case RET: strcpy(name, "ret"); break;
		case WRITEB: strcpy(name, "writeb"); break;
		case WRITEL: strcpy(name, "writel"); break;
		case READB: strcpy(name, "readb"); break;
		case READL: strcpy(name, "readl"); break;
		case MOVSBL: strcpy(name, "movsbl"); break;
		default:
			sprintf(name, "0X%0x", opcode);
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

int getRegisterId(char *str){
	if(strcmp(str, "%eax") == 0) return EAX;
	if(strcmp(str, "%ebx") == 0) return EBX;
	if(strcmp(str, "%ecx") == 0) return ECX;
	if(strcmp(str, "%edx") == 0) return EDX;
	if(strcmp(str, "%esp") == 0) return ESP;
	if(strcmp(str, "%ebp") == 0) return EBP;
	if(strcmp(str, "%esi") == 0) return ESI;
	if(strcmp(str, "%edi") == 0) return EDI;
	return 15;
}
