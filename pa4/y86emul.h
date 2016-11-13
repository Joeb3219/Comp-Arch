#ifndef Y86EMUL_H
#define Y86EMUL_H

#define DEBUG 1 // Can be 0 for no output, 1 for important output, or 2 for all output

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
        unsigned int rA, rB, d;
} Instr;


#endif
