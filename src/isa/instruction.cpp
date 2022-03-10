#include "instruction.h"
#include "isa.h"
#include "cpu.h"
#include <iostream>

Instruction_B::Instruction_B(OPCODE _opcode, REGISTER_ABI_NAME _rs1, REGISTER_ABI_NAME _rs2, int32_t _imm)
{
    opcode = _opcode;
    rs2 = _rs2;
    rs1 = _rs1;
    rd = PC;
    imm = _imm;

    requiresMemoryAccess = false;
    requiresWriteBack = true;
}
int32_t Instruction_B::getImm()
{
    return imm;
}

void Instruction_B::setImm(int32_t val)
{
    imm = val;
}

Instruction_I::Instruction_I(OPCODE _opcode, REGISTER_ABI_NAME _rd, REGISTER_ABI_NAME _rs1, int32_t _imm)
{
    opcode = _opcode;
    rd = _rd;
    rs1 = _rs1;
    rs2 = ZERO;
    imm = _imm;

    switch (opcode)
    {
    case LW: // Loads are the only implmented I-format instructions that require a memory access
        requiresMemoryAccess = true;
        break;
    default:
        requiresMemoryAccess = false;
        break;
    }
    requiresWriteBack = true;
}

Instruction_R::Instruction_R(OPCODE _opcode, REGISTER_ABI_NAME _rd, REGISTER_ABI_NAME _rs1, REGISTER_ABI_NAME _rs2)
{
    opcode = _opcode;
    rd = _rd;
    rs1 = _rs1;
    rs2 = _rs2;
    imm = 0;

    requiresMemoryAccess = false;
    requiresWriteBack = true;
}

Instruction_S::Instruction_S(OPCODE _opcode, REGISTER_ABI_NAME _rs1, REGISTER_ABI_NAME _rs2, int32_t _imm)
{
    opcode = _opcode;
    rd = ZERO;
    rs2 = _rs2;
    rs1 = _rs1;
    imm = _imm;

    requiresMemoryAccess = true;
    requiresWriteBack = false; // No implemented stores require a writeback
}

Instruction_Halt::Instruction_Halt()
{
    opcode = HLT;
    rd = ZERO;
    rs1 = ZERO;
    rs2 = ZERO;
    requiresMemoryAccess = false;
    requiresWriteBack = false;
}