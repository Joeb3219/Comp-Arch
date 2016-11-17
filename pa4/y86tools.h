#ifndef Y86TOOLS_H_
#define Y86TOOLS_H_

#define EAX 0
#define ECX 1
#define EDX 2
#define EBX 3
#define ESP 4
#define EBP 5
#define ESI 6
#define EDI 7

typedef enum opcodes{
        NOP = 0x00, HALT = 0x10, RRMOVL = 0x20, IRMOVL = 0x30, RMMOVL = 0x40, MRMOVL = 0x50,
        JMP = 0x70, JLE = 0x71, JL = 0x72, JE = 0x73, JNE = 0x74, JGE = 0x75, JG = 0x76,
        CALL = 0x80, RET = 0x90, PUSHL = 0xA0, POPL = 0xB0,
        READB = 0xC0, READL = 0xC1, WRITEB = 0xD0, WRITEL = 0xD1,
        MOVSBL = 0xE0,
        ADDL = 0x60, SUBL = 0x61, ANDL = 0x62, XORL = 0x63, MULL = 0x64, CMPL = 0x65
} Opcode;

typedef enum status{
        AOK = 0, HLT = 1, ADR = 2, INS = 3
} Status;

typedef struct instruction{
        Opcode opcode;
        int args;
        int rA, rB, d;
} Instr;



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
