#ifndef ISA_H
#define ISA_H

#include <cinttypes>

enum OPCODE
{
    LOAD,
    STORE,
    ADD,
    SUB,
    ADDI,
    HLT,

    NOP,
};

// https://en.wikichip.org/wiki/risc-v/registers
enum REGISTER
{
    // X0 hardwired to constant zero (RISC-V)
    X0,

    // X1 - X31 General purpose integer registers
    X1,
    X2,
    X3,
    X4,
    X5,
    X6,
    X7,
    X8,
    X9,
    X10,
    X11,
    X12,
    X13,
    X14,
    X15,
    X16,
    X17,
    X18,
    X19,
    X20,
    X21,
    X22,
    X23,
    X24,
    X25,
    X26,
    X27,
    X28,
    X29,
    X30,
    X31,

    // Program Counter
    PROGRAM_COUNTER,

    REG_UNUSED // NOT USED, just provides a value for the number of registers
};

// Register ABI names corresponding to the REGISTERS enum
enum REGISTER_ABI_NAME
{
    ZERO, // Hardwired zero
    RA,   // Return address
    SP,   // Stack Pointer
    GP,   // Global Pointer
    TP,   // Thread Pointer
    T0,
    T1,
    T2, // Temporary registers (X5-7)
    FP, // AKA S0, Frame pointer/Saved register
    S1, // Saved register
    A0,
    A1, // Function arguments / return values

    // Function Arguments
    A2,
    A3,
    A4,
    A5,
    A6,
    A7,

    // Saved Registers
    S2,
    S3,
    S4,
    S5,
    S6,
    S7,
    S8,
    S9,
    S10,
    S11,

    // Temporary Registers
    T3,
    T4,
    T5,
    T6,

    PC, // Program counter

    REGABI_UNUSED // NOT USED, just provides a value for the number of registers
};

// https://github.com/jameslzhu/riscv-card/blob/master/riscv-card.pdf
enum INSTRUCTION_FORMAT
{
    R,
    I,
    S,
    B,
    U,
    J,
    PSEUDO,
    SPECIAL,

    INSTRUCTION_FORMAT_COUNT // unused
};

extern int32_t registers[REG_UNUSED];

// Instructions as CPU operations:
void CPU_Add(REGISTER_ABI_NAME rd, REGISTER_ABI_NAME rs1, REGISTER_ABI_NAME rs2);
void CPU_Sub(REGISTER_ABI_NAME rd, REGISTER_ABI_NAME rs1, REGISTER_ABI_NAME rs2);
void CPU_AddI(REGISTER_ABI_NAME rd, REGISTER_ABI_NAME rs1, int32_t imm);
void CPU_Halt();

class Instruction
{
private:
    INSTRUCTION_FORMAT format; // not sure if needed
    REGISTER_ABI_NAME rs1;
    REGISTER_ABI_NAME rs2;
    REGISTER_ABI_NAME rd;
    int imm;

public:
    OPCODE opcode;

    Instruction();
    // R-type instruction constructor
    Instruction(OPCODE _opcode, INSTRUCTION_FORMAT _format, REGISTER_ABI_NAME _rd, REGISTER_ABI_NAME _rs1, REGISTER_ABI_NAME _rs2)
    {
        format = _format;
        opcode = _opcode;
        rs1 = _rs1;
        rs2 = _rs2;
        rd = _rd;
    }

    // I-type, S-type, B-type instruction all have the same signature
    Instruction(OPCODE _opcode, INSTRUCTION_FORMAT _format, REGISTER_ABI_NAME _rd, REGISTER_ABI_NAME _rs1, int32_t _imm)
    {
        format = _format;
        opcode = _opcode;
        rs1 = _rs1;
        rd = _rd;

        // Imm shoud be different lengths depending on format (see spec), TODO (maybe): add some stuff to make that happen
        imm = _imm;
    }

    Instruction(OPCODE _opcode, INSTRUCTION_FORMAT _format)
    {
        format = _format;
        opcode = _opcode;
    }

    void Execute()
    {
        switch (format)
        {
        case R:
            Execute_R();
            break;
        case I:
            Execute_I();
            break;
        case S:
            break;
        case B:
            break;
        default:
            CPU_Halt();
            break;
        }
    }

private:
    void Execute_R()
    {
        switch (opcode)
        {
        case ADD:
            CPU_Add(rd, rs1, rs2);
            break;

        default:
            break;
        }
    }

    void Execute_I()
    {
        switch (opcode)
        {
        case ADDI:
            CPU_AddI(rd, rs1, imm);
            break;

        default:
            break;
        }
    }
};

#endif