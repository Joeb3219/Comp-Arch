#ifndef Y86EMUL_H
#define Y86EMUL_H

#define DEBUG 1 // Can be 0 for no output, 1 for important output, or 2 for all output

typedef enum opcodes{
        NOP = 0x0, HALT = 0x1, RRMOVL = 0x2, IRMOVL = 0x3, RMMOVL = 0x4, MRMOVL = 0x5,
        JMP = 0x6, JLE = 0x7, JL = 0x8, JE = 0x9, JNE = 0xA, JGE = 0xB, JG = 0xC,
        CALL = 0xD, RET = 0xE, PUSHL = 0xF, POPL = 0x10,
        READB = 0x11, READL = 0x12, WRITEB = 0x13, WRITEL = 0x14,
        MOVSBL = 0x15,
        ADDL = 0x16, SUBL = 0x17, ANDL = 0x18, XORL = 0x19, MULL = 0x1A, CMPL = 0x1B
} Opcode;

typedef enum status{
        AOK = 0, HLT = 1, ADR = 2, INS = 3
} Status;

typedef struct instruction{
        Opcode opcode;
        int args;
        unsigned int *operands;
} Instr;


#endif
