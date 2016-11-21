#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "y86tools.h"
#include "futil.h"

static char **symbols;
static int symbolCount = 0, *symbolValues;

int processImmediate(char *imm){
	imm = imm + 1;								// Get rid of $
	if(strlen(imm) <= 2) return strtol(imm, NULL, 10);			// Base 10 if it's <= 2 characters.
	if(imm[1] == 'X' || imm[1] == 'x') return strtol(imm + 2, NULL, 16);	// Base 16
	if(imm[0] == '0') return strtol(imm + 1, NULL, 8);			// Base 8
	return strtol(imm, NULL, 10);						// Default, base 10
}

int getSymbolId(char *symbol){
        int i = 0;
        for(i = 0; i <= symbolCount; i ++){
                if(symbols[i] == 0) continue;
                if(strcmp(symbols[i], symbol) == 0) return i;
        }
        return -1;
}

void addSymbol(char *symbol){
        symbols = realloc(symbols, sizeof(char**) * (symbolCount + 1));
        symbolValues = realloc(symbolValues, sizeof(int) * (symbolCount + 1));
        symbols[symbolCount] = strdup(symbol);
        symbolCount ++;
}

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

void findSymbols(FILE *file){
	char *token;
	int instrCount = 0, countRegs = 1;
        while( strlen((token = getNextToken(file))) != 0){
		if(token[0] == '$') instrCount += 4;
                else if(token[0] == '.'){
			if(token[strlen(token) - 1] == ':'){
				token[strlen(token) - 1] = '\0';
				setSymbolValue(token, instrCount);
			}
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
}

void storeInstruction(char *buffer, Instr *instr){
	
}

void decipher(FILE *file, char *program){
	char *token, buffer[9];
	Instr *instr = malloc(sizeof(Instr));
	instr->opcode = instr->rA = instr->rB = instr->d = -1;
	while( strlen((token = getNextToken(file))) != 0){
		if(token[0] == '$'){
			printLittleEndian(buffer, processImmediate(token));
			buffer[8] = '\0';
			//strcat(program, buffer);
		}else if(token[0] == '.'){
			if(token[strlen(token) - 1] == ':') token[strlen(token) - 1] = '\0';
			printLittleEndian(buffer, getSymbolValue(token));
			buffer[8] = '\0';
			//strcat(program, buffer);
		}
		else if(token[0] == '%'){
			sprintf(buffer, "%0X", getInstructionOpcode(token));
			buffer[1] = '\0';
			//strcat(program, buffer);
		}
		else{
			sprintf(buffer, "%02X\n", getInstructionOpcode(token));
			buffer[2] = '\0';
			//strcat(program, buffer);
		}
		printf("%s ", buffer);
		free(token);
	}
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

	char *program = malloc(sizeof(char) * 800);

	symbols = malloc(sizeof(char**));
	symbolValues = malloc(sizeof(int));

	findSymbols(file);
	fseek(file, 0L, SEEK_SET);
	decipher(file, program);

	printf(".text\t0\t%s\n", program);

	closeFile(file);

	return 0;
}
