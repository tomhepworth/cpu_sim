#include "isa.h"
#include <stdio.h>
#include "cpu.h"
#include <iostream>

int32_t registers[REG_UNUSED]; // extern in isa.h

Instruction::Instruction()
{
    opcode = NOP;
}

Instruction::~Instruction()
{
    delete[] free;
}

// Default implementation for fetch
bool Instruction::fetch(CPU *cpu, Scoreboard *scoreboard)
{
    // Branch instruction may change the PC, so check that it's not being messed with
    if (!scoreboard->isValid(PC))
        return false;

    // mark fetch as completed
    free[FETCH] = true;
    return true;
}

// Default implementation for decode
bool Instruction::decode(CPU *cpu, Scoreboard *scoreboard)
{
    // If previous pipeline stage not completed we cant do this one
    if (!free[FETCH])
        return false;

    // Mark this stage as completed so instruction can advance in pipeline
    free[DECODE] = true;
    return true;
}

// Default implementation for execute
bool Instruction::execute(CPU *cpu, Scoreboard *scoreboard)
{
    // If previous pipeline stage not completed we cant do this one
    if (!free[DECODE])
        return false;

    // Mark this stage as completed so instruction can advance in pipeline
    free[EXECUTE] = true;
    return true;
}

// Default implementation for memoryAccess
bool Instruction::memoryAccess(CPU *cpu, Scoreboard *scoreboard)
{
    // If previous pipeline stage not completed we cant do this one
    if (!free[EXECUTE])
        return false;

    // Mark this stage as completed so instruction can advance in pipeline
    free[MEMORY] = true;
    return true;
}

// Default implementation for writeBack
bool Instruction::writeBack(CPU *cpu, Scoreboard *scoreboard)
{
    // If previous pipeline stage not completed we cant do this one
    if (!free[MEMORY])
        return false;

    // Mark this stage as completed so instruction can advance in pipeline
    free[WRITEBACK] = true;
    return true;
}

void Instruction::reset()
{
    for (size_t i = 0; i < STAGE_COUNT; i++)
    {
        free[i] = false;
    }
}

// void Instruction::setImm(int32_t _imm)
// {
//     printf("Warning: setImm not implemented for base instruction class\n");
// }

// int32_t Instruction::getImm()
// {
//     printf("Warning: getImm not implemented for base instruction class\n");
//     return 0;
// }

int PerformALUOperation(OPCODE opcode, int32_t val1, int32_t val2)
{
    int32_t result;
    switch (opcode)
    {
    case LW: // Loads and stores need to add offset to rs1
    case SW:
    case ADD:
    case ADDI:
        result = val1 + val2;
        break;
    case SUB:
        result = val1 - val2;
        break;
    case SLL:
        result = val1 << val2;
        break;
    case SRL:
        result = val1 >> val2;
        break;
    case AND:
        result = val1 & val2;
        printf("ANDING %d and %d for RES: %d\n", val1, val2, result);
        break;
    case OR:
        result = val1 | val2;
        break;
    case XOR:
        result = val1 ^ val2;
        break;
    default:
        break;
    }

    return result;
}