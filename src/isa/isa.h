#ifndef ISA_H
#define ISA_H

#include <cinttypes>
#include <assert.h>

typedef int32_t memory_address;

// All the opcodes supported by the cpu
enum OPCODE
{
    NOP, // Keep nop zero for some pipeline logic
    LOAD,
    STORE,
    ADD,
    SUB,
    ADDI,
    HLT,
};

// Execution stages
enum CPU_STAGE
{
    FETCH,
    DECODE,
    EXECUTE,
    MEMORY,
    WRITEBACK,
    STAGE_COUNT // Not used, just equals number of stages
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

// This function models all the ALU operations as generally as I can (IE, adds are all combined)
int PerformALUOperation(OPCODE opcode, int32_t val1, int32_t val2);

// This function models all the ALU operations as generally as I can (IE, adds are all combined)
int PerformMemoryOperation(OPCODE opcode, memory_address destination, int32_t value);

#endif