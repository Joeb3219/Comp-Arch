#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "y86tools.h"
#include "futil.h"

// This program goes through all of the symbols in a file and converts them into opcode form.
// This occurs by creating a symbol table for text references (function names, etc), and converting them into addresses.
// Thus, the process is a two step system.
// First, we analyze all symbols, then replace them.
// Then we can convert all instructions into numeric versions.

static char **symbols;
static int symbolCount = 0, *symbolValues;

int processImmediate(char *imm){
	if(imm[0] == '$') imm = imm + 1;					// Get rid of $
	if(strlen(imm) <= 2) return strtol(imm, NULL, 10);			// Base 10 if it's <= 2 characters.
	if(imm[1] == 'X' || imm[1] == 'x') return strtol(imm + 2, NULL, 16);	// Base 16
	if(imm[0] == '0') return strtol(imm + 1, NULL, 8);			// Base 8
	return strtol(imm, NULL, 10);						// Default, base 10
}

// Converts a character symbol into its corresponding id.
int getSymbolId(char *symbol){
        int i = 0;
        for(i = 0; i <= symbolCount; i ++){
                if(symbols[i] == 0) continue;
                if(strcmp(symbols[i], symbol) == 0) return i;
        }
        return -1;
}

// Adds a symbol to the list.
void addSymbol(char *symbol){
        symbols = realloc(symbols, sizeof(char**) * (symbolCount + 1));
        symbolValues = realloc(symbolValues, sizeof(int) * (symbolCount + 1));
        symbols[symbolCount] = strdup(symbol);
        symbolCount ++;
}

// Sets the value of a symbol.
void setSymbolValue(char *symbol, int val){
	int id = getSymbolId(symbol);
	if(id == -1){
		id = symbolCount;
		addSymbol(symbol);
	}
	symbolValues[id] = val;
}

int getSymbolValue(char *symbol){
	int id = getSymbolId(symbol);
	if(id == -1) return -1;
	return symbolValues[id];
}


// Looks through a file and identifies all symbols which are text and not instructions. 
// Symbols begin with a '.'
// The purpose of this is to eventually replace all symbols with their address equivalent.
// Thus, we will convert every ".symbol" to the address at which ".symbol:" first appears.
void findSymbols(FILE *file){
	char *token;
	int instrCount = 0, countRegs = 1;
        while( strlen((token = getNextToken(file))) != 0){
		if(token[0] == '$'){
			instrCount += 4;
                	if(strchr(token, '(')){
				instrCount ++;
				countRegs = 0;
			}
		}
		else if(token[0] >= '0' && token[0] <= '9'){
			instrCount += 5;
			countRegs = 0;
		}
		else if(token[0] == '.'){
			if(token[strlen(token) - 1] == ':'){
				token[strlen(token) - 1] = '\0';
				setSymbolValue(token, instrCount);
			}
			else instrCount += 4;
		}
                else if(token[0] == '%'){
			if(countRegs == 1){
				instrCount += 1;
                		countRegs = 0;
			}
		}else{
			instrCount += 1;
                	countRegs = 1;
		}
		free(token);
        }
        free(token);
}

void printLittleEndian(char *buffer, int val){
	int i = 0, j = 0;
        char res[9];
        sprintf(&res[0], "%08x", val);
        for(i = 7; i >= 1; i -= 2){
		buffer[j + 1] = res[i];
		buffer[j] = res[i - 1];
        	j += 2;
	}
	buffer[8] = '\0';
}

void storeInstruction(char *program, Instr *instr){
	if(instr->rA == -1) instr->rA = 15;
	if(instr->rB == -1) instr->rB = 15;
	if(instr->d == -1) instr->d = 0;
	char buffer[9];
	sprintf(buffer, "%02X", instr->opcode);
	buffer[3] = '\0';
	strcat(program, buffer);
	switch(instr->opcode){
		case RRMOVL:
		case ADDL:
		case SUBL:
		case MULL:
		case XORL:
		case ANDL:
		case CMPL:
			append(program, digToHexChar(instr->rA));
			append(program, digToHexChar(instr->rB));
			break;
		case MOVSBL:
		case MRMOVL:
                        append(program, digToHexChar(instr->rB));
			append(program, digToHexChar(instr->rA));
			printLittleEndian(buffer, instr->d);
			strcat(program, buffer);
			break;
		case RMMOVL:
			append(program, digToHexChar(instr->rA));
			append(program, digToHexChar(instr->rB));
                        printLittleEndian(buffer, instr->d);
                        strcat(program, buffer);
			break;
		case IRMOVL:
                        append(program, digToHexChar(instr->rB));
			append(program, digToHexChar(instr->rA));
                        printLittleEndian(buffer, instr->d);
                        strcat(program, buffer);
                        break;
                case READB:
                case WRITEB:
                case READL:
                case WRITEL:
			append(program, digToHexChar(instr->rA));
			append(program, digToHexChar(instr->rB));
			printLittleEndian(buffer, instr->d);
                        strcat(program, buffer);
			break;
		case PUSHL:
		case POPL:
			append(program, digToHexChar(instr->rA));
			append(program, digToHexChar(instr->rB));
			break;
		case CALL:
		case JMP:
		case JE:
		case JNE:
		case JG:
		case JGE:
		case JL:
		case JLE:
			printLittleEndian(buffer, instr->d);
                        strcat(program, buffer);
			break;
		default: break;
	}
//	append(program, ' ');
}

// Now that we have identified all symbols and calculated their address, we can replace them in the file and then convert as normal.
// This conversion is literally matching what we expect of an instruction, given its mnemonic. 
// We attempt to build an instruction struct, and then store it as we go.
void decipher(FILE *file, char *program){
	char *token, *subtoken;
	Instr *instr = malloc(sizeof(Instr));
	instr->opcode = instr->rA = instr->rB = instr->d = -1;
	while( strlen((token = getNextToken(file))) != 0){
		// Check if current token is an immediate.
		if(token[0] == '$' || (token[0] >= '0' && token[0] <= '9')){
			subtoken = strchr(token, '(');
			if(subtoken == 0){
				instr->d = processImmediate(token);
				free(token);
				continue;
			}
			if(subtoken[0] == '('){
                                subtoken = subtoken + 1;
                                subtoken[strlen(subtoken) - 1] = '\0';
                        }
                        if(instr->rA == -1) instr->rA = getRegisterId(subtoken);
                        else instr->rB = getRegisterId(subtoken);
			token[subtoken - token] = '\0';
                        instr->d = processImmediate(token);
		}
		// Check if current token is a symbol. If so, store the address value associated with the symbol.
		else if(token[0] == '.'){
			if(token[strlen(token) - 1] == ':') continue;	// If the symbol is a definition (ie: ends in ':'), skip it.
			instr->d = getSymbolValue(token);
		}
		// Check if the token is a register or an immediate wrapping a register.
		else if(token[0] == '%' || token[0] == '('){
			if(token[0] == '('){
				subtoken = token + 1;
				subtoken[strlen(subtoken) - 1] = '\0';
			}else subtoken = token;
			if(instr->rA == -1) instr->rA = getRegisterId(subtoken);
			else instr->rB = getRegisterId(subtoken);
		// Otherwise, this is an instruction mnemonic!
		}else{
			if(instr->opcode != -1){
				storeInstruction(program, instr);
				free(instr);
				instr = malloc(sizeof(Instr));
				instr->opcode = instr->rA = instr->rB = instr->d = -1;
			}
			instr->opcode = getInstructionOpcode(token);
		}
		free(token);
	}
	if(instr->opcode != -1) storeInstruction(program, instr);
	free(token);
}

int main(int argc, char **argv){
	if(argc != 2){
		printf("USAGE: y86asmbl <filename>\n");
		return 1;
	}

	FILE *file = getFile(argv[1]);
	if(file == 0){
		printf("File %s not found.\n", argv[1]);
		return 1;
	}

	FILE *output = getFileOrCreate(strcat(argv[1], "_gen"));

	char *program = calloc(800, sizeof(char));

	symbols = malloc(sizeof(char**));
	symbolValues = malloc(sizeof(int));

	findSymbols(file);		// Find all symbols in file
	fseek(file, 0L, SEEK_SET);	// Revert file pointer back to the beginning of the file.
	decipher(file, program);	// Decipher, replacing symbols with integers and mnemonics and other numbers with hex equivalents.

	printf(".text\t0\t%s\n", program);	// Print the text directive
	fprintf(output, ".text\t0\t%s\n", program);	// Store the text directive.

	printf("Generated a file with contents: %s\n", argv[1]);

	free(program);

	closeFile(file);

	return 0;
}
