#include "isa.h"
#include <stdio.h>
#include "main.h"
#include "cpu.h"

int32_t registers[REG_UNUSED]; // extern in isa.h

Instruction::Instruction()
{
    opcode = NOP;
}

// Default implementation for fetch
bool Instruction::fetch(Scoreboard *scoreboard)
{
    // mark fetch as completed
    free[FETCH] = true;
    return true;
}

// Default implementation for decode
bool Instruction::decode(Scoreboard *scoreboard)
{
    // If previous pipeline stage not completed we cant do this one
    if (!free[FETCH])
        return false;

    // Mark this stage as completed so instruction can advance in pipeline
    free[DECODE] = true;
    return true;
}

// Default implementation for execute
bool Instruction::execute(Scoreboard *scoreboard)
{
    // If previous pipeline stage not completed we cant do this one
    if (!free[DECODE])
        return false;

    // Mark this stage as completed so instruction can advance in pipeline
    free[EXECUTE] = true;
    return true;
}

// Default implementation for memoryAccess
bool Instruction::memoryAccess(Scoreboard *scoreboard)
{
    // If previous pipeline stage not completed we cant do this one
    if (!free[EXECUTE])
        return false;

    // Mark this stage as completed so instruction can advance in pipeline
    free[MEMORY] = true;
    return true;
}

// Default implementation for writeBack
bool Instruction::writeBack(Scoreboard *scoreboard)
{
    // If previous pipeline stage not completed we cant do this one
    if (!free[MEMORY])
        return false;

    // Mark this stage as completed so instruction can advance in pipeline
    free[WRITEBACK] = true;
    return true;
}

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
    default:
        break;
    }

    return result;
}