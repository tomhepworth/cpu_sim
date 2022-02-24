#ifndef TEST_MODE
#include "debug_utils.h"
#endif

#include "isa.h"
#include "instruction.h"
#include "scoreboard.h"
#include "cpu.h"

#include "stdio.h"
#include <iostream>

Instruction_I::Instruction_I(OPCODE _opcode, REGISTER_ABI_NAME _rd, REGISTER_ABI_NAME _rs1, int32_t _imm)
{
    opcode = _opcode;
    rd = _rd;
    rs1 = _rs1;
    imm = _imm;
}

bool Instruction_I::decode(CPU *cpu, Scoreboard *scoreboard)
{
    // If the instruction is not fetched, we cant decode
    if (!free[FETCH])
        return false;

    // Determine if we have the value of rs1
    // Lookup in scoreboard, if we can get it, nice

    if (scoreboard->isValid(rs1) && scoreboard->isValid(rd))
    {
        // Set RD to invalid so that it is reserved for this instruction to use
        scoreboard->setInvalid(rd);

        rs1Value = cpu->registers[rs1];
        gotRs1 = true;
    }

    // Only mark that we are free to advance in pipeline if we have everything we need to do so
    if (gotRs1)
    {
        free[DECODE] = true;
        return true;
    }

    return false; // Default;
}

bool Instruction_I::execute(CPU *cpu, Scoreboard *scoreboard)
{
    // If the instruction is not decoded, we cant execute;
    if (!free[DECODE])
        return false;

    assert(gotRs1);

    // Now do the ALU operation
    rdValue = PerformALUOperation(opcode, rs1Value, imm);

    // if (debug)
    //     printf("LW: Setting rd (%d) to %d %d\n", rd, rdValue, imm);

    // Mark that we are free to advance in pipeline
    free[EXECUTE] = true;

    return true;
}

bool Instruction_I::memoryAccess(CPU *cpu, Scoreboard *scoreboard)
{
    // If previous pipeline stage not completed we cant do this one
    if (!free[EXECUTE])
        return false;

    /*
        Switch on opcode to decide if instruction requires memory access.
        For loads ( `lw rd, rs1, imm` or `lw rd, offset(rs1)`) use rdValue as memory address
        For normal (`addi rd rs1 imm`) use rdValue as result

        lw rd, rs1, imm ==> rd = Memory[rs1 +]
        addi rd rs1 imm ==> rd = rs1 + imm;
    */
    switch (opcode)
    {
    case LW:
    {
        int32_t addr = rdValue;
        rdValue = cpu->memory[addr];
        printf("LW: Loaded %d from mem[%d]\n", rdValue, addr);
        break;
    }
    default:
        // Do nothing, already sorted in execute stage
        break;
    }

    // Mark this stage as completed so instruction can advance in pipeline
    free[MEMORY] = true;
    return true;
}

bool Instruction_I::writeBack(CPU *cpu, Scoreboard *scoreboard)
{
    // If the instruction has completed the memory stage
    if (!free[MEMORY])
        return false;

    // Write rdValue to result register, handling scoreboard stuff
    cpu->registers[rd] = rdValue;
    rdWritten = true;

#ifndef TEST_MODE
    if (debug)
        printf("Setting rd (%d) to %d\n", rd, rdValue);
#endif

    scoreboard->setValid(rd); // mark are done with rd

    // Mark we are complete in the writeback phase
    free[WRITEBACK] = true;
    return true;
}

void Instruction_I::reset()
{
    for (size_t i = 0; i < STAGE_COUNT; i++)
    {
        free[i] = false;
    }
    gotRs1 = false;
}