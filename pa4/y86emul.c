#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include "y86emul.h"
#include "futil.h"
#include "y86tools.h"

// Fetches the current instruction and then increments the count by one.
int fetch(){
	int val = count;
	count += 1;
	return val;
}

// Converts the instruction at the indicated address to an Instruction structure.
// LoadArgs will add the data around addy to the instruction, which is utilized per instrution description.
Instr* decode(int addy){
	Instr *instr = malloc(sizeof(Instr));
	instr->opcode = getMemory(addy);
	loadArgs(instr, addy);
	return instr;
}

// Executes an instruction.
// This function will incremement count by an amount equal to the number of parameter bytes used.
// For example, if an instruction uses just registers, it will increment by 1 byte.
// If an instruction uses a 32-bit word, it will increment by 5 bytes.
// If both a word and registers are used in instruction, will incremement by 6 bytes.
// Thus, after execute, count will point to the correct instruction for the next cycle.
void execute(Instr* instr){
	int a, b, x;								// a, b, and x are used to calculate if an overflow occured.
	char buffer[9];								// A buffer to convert integers to string representations.
	if(DEBUG >= 2) printInstruction(instr, stdout);
	switch(instr->opcode){
		case NOP:							// No operation, just continue.
			break;
		case HALT:							// Halt, signal to the CPU that we are done via the HLT signal.
			status = HLT;
			break;
		case RRMOVL:							// RRMOVL, increment count by one (we use one byte of data)
			count += 1;
                        setRegister(instr->rB, getRegister(instr->rA));
			break;
                case IRMOVL:							// IRMOVL, increment count by 5 (we use 5 bytes of data).
			count += 5;
			setRegister(instr->rB, instr->d);
                        break;
                case RMMOVL:							// RRMOVL, increment count by 5.
			count += 5;
                        setLong(getRegister(instr->rB) + instr->d, getRegister(instr->rA));
			break;
                case MRMOVL:							// MRMOVL, increment count by 5.
			count += 5;
                        setRegister(instr->rA, getLong(instr->d + getRegister(instr->rB)));
                        break;
                case JMP:							// JMP, so just move to the indicated word.
			count = instr->d;
			break;
		// For all Jump instructions other than JMP, we either set count to the indicated word if conditions are met,
		// or we increment count by 4 to bypass the word, such that we go to the next instruction.
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
                case CALL:							// Push the next instruction (4 bytes ahead), and then change count to the given word.
			push(count + 4);
			count = instr->d;
                        break;
                case RET:							// RET returns back to the previously pushed count.
			count = pop();
			break;
                case PUSHL:							// PUSHL, increment count by one byte.
			count ++;
                        push(getRegister(instr->rA));
			break;
                case POPL:							// POPL, incremement count by one byte.
			count ++;
			setRegister(instr->rA, pop());
                        break;
                case READB:							// READB, incremement count by 5.
			count += 5;
                        ZF = (0 == read(STDIN_FILENO, buffer, 2));					// If read returns zero, we have no text left to parse.
                        buffer[2] = buffer[3] = buffer[4] = buffer[5] = buffer[6] = buffer[7] = 0;	// Read in one byte, and set the rest of the buffer to 0.
			buffer[8] = '\n';
			setLong(instr->d + getRegister(instr->rA), strtol(buffer, NULL, 10));		// We now parse this byte.
			break;
                case READL:
			count += 5;						// READL, incrememnt count by 5.
			ZF = (read(STDIN_FILENO, buffer, 8) == 0);					// If read returns zero, we have no text left to parse.
			buffer[8] = '\n';
			setLong(instr->d + getRegister(instr->rA), strtol(buffer, NULL, 10));		// Parse the 4 input bytes.
			break;
                case WRITEB:
			count += 5;									// WRITEB, increment by 5 bytes.
			printf("%c", getMemory(instr->d + getRegister(instr->rA)));			// Simply print the character.
			fflush(stdout);									// Flush to ensure all output is pushed out.
			break;
                case WRITEL:
			count += 5;									// WRITEL, increment by 5 bytes.
			a = getLong(getRegister(instr->rA) + instr->d);					// Get the long at the address, print it.
			fflush(stdout);									// Flushes the output.
			printf("%d\n", a);
			fflush(stdout);									// Flushes the output.
                        break;
		// ALL Math instructions utilize one additional byte (the registers), and set status conditions.
                case ADDL:
			count += 1;
			a = getRegister(instr->rA);
			b = getRegister(instr->rB);
			x = a + b;
			OF = ((a > 0 && b > INT_MAX - a) || (a < 0 && b < INT_MIN - a)); 		// See if a + b > INT_MAX
			SF = (x < 0);
			ZF = (x == 0);
			setRegister(instr->rB, x);
                        break;
                case SUBL:
			count += 1;
			a = getRegister(instr->rA);
			b = getRegister(instr->rB);
			x = b - a;
			OF = ((a > 0 && b > INT_MAX - a) || (-a < 0 && b < INT_MIN - a));		// See if a - b < INT_MAX
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
			OF = (a != 0 && ((x / a) != b));						// See if (a*b)/a is equal to b (if not, we have a math overflow)
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
			OF = ((a > 0 && b > INT_MAX - a) || (a < 0 && b < INT_MIN - a));		// Uses a similar ideology as SUBL.
			break;
		case MOVSBL:										// MOVSBL, increment count by 5.
			count += 5;
			setRegister(instr->rA, getMemory(getRegister(instr->rB) + instr->d));
			break;
		default:										// A failsafe, used for debugging. Ceases program execution.
			printf("UNKNOWN @ %d: %02X\n", count, instr->opcode);
			status = INS;
			break;
	}
}

// The CPU execution
void executeProgram(){
	do{
		Instr *instr = decode(fetch());								// decode the current instruction
		execute(instr);										// execute it
		free(instr);										// Get rid of our reference to that instruction
	}while(status == AOK);										// Go until we're not ok anymore (like in real life)
	if(status != HLT) printf("Encountered a non-halt status condition: %d\n", status);
}

int main(int argc, char **argv){
	FILE *file;

	// Check for argument count.
	if(argc != 2){
		printf("Unexpected input count: %d. Use -h for help\n", argc);
		return 1;
	}
	// Check if help flag included.
	if(strcmp(argv[1], "-h") == 0){
		printf("USAGE: ./y86emul <filename>\n");
		return 1;
	}

	// Get the indicated file, and check if it exists.
	file = getFile(argv[1]);
	if(file == 0){
		printf("File not found: %s\n", argv[1]);
		return 1;
	}

	// Attempt to parse the program. If there is an error, die before execution (as opposed to execute before dying?)
	if(loadProgramIntoMemory(file) == -1){
		printf("Error processing file.\n");
		return 1;
	}

	executeProgram();

	closeFile(file);	// Free up that glorious file memory.

	free(memory);		// Free up this memory.

	return 0;
}
