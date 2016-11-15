#ifndef Y86TOOLS_H_
#define Y86TOOLS_H_
int hexCharToDig(char c);
void printInstruction(Instr *instr, FILE *file);
char digToHexChar(unsigned char d);
char* getInstructionName(Opcode opcode);
void appendArguments(char *buffer, Instr *instr);
char* getRegisterName(int id);
void printMemory(unsigned char *memory, int size, int chars);
#endif
