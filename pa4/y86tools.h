#ifndef Y86TOOLS_H_
#define Y86TOOLS_H_

extern int *registers;
extern unsigned char *memory;
extern int OF, ZF, SF;
extern int count, memorySize;
extern Status status;

void push(int val);
int pop();
void loadArgs(Instr *instr, int addy);

void createRegisters(int num);
void setRegister(int id, int val);
int getRegister(int id);
void setMemory(int id, int val);
int getMemory(int id);
void setLong(int id, int val);
int interpretLong(int startingAddy);
int getLong(int id);
void loadByteIntoMemory(char *address, char *value);
void loadLongIntoMemory(char *address, char *value);
void loadStringIntoMemory(char *address, char *string);
int loadProgramIntoMemory(FILE *file);
int setInstructions(char *address, char *instructions);
int setMemorySize(char *size);

int hexCharToDig(char c);
void printInstruction(Instr *instr, FILE *file);
char digToHexChar(unsigned char d);
char* getInstructionName(Opcode opcode);
void appendArguments(char *buffer, Instr *instr);
char* getRegisterName(int id);
void printMemory(unsigned char *memory, int size, int chars);
#endif
