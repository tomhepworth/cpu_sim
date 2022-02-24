#include "isa.h"
#include "instruction.h"
#include "scoreboard.h"
#include "cpu.h"
#include "stdio.h"
#include <iostream>

Instruction_R::Instruction_R(OPCODE _opcode, REGISTER_ABI_NAME _rd, REGISTER_ABI_NAME _rs1, REGISTER_ABI_NAME _rs2)
{
    opcode = _opcode;
    rd = _rd;
    rs1 = _rs1;
    rs2 = _rs2;
}

bool Instruction_R::decode(CPU *cpu, Scoreboard *scoreboard)
{
    // If the instruction is not fetched, we cant decode
    if (!free[FETCH])
        return false;

    // Determine if we have the values of rs1 and rs2
    // Lookup in register file, if we can get it, nice
    if (scoreboard->isValid(rs1) && scoreboard->isValid(rs2))
    {
        printf("Setting GOTs for %d\n", linenum);
        rs1Value = cpu->registers[rs1];
        gotRs1 = true;

        rs2Value = cpu->registers[rs2];
        gotRs2 = true;

        // Ready to execute so set rd to be invalid in scoreboard (!!!)
        scoreboard->setInvalid(rd);
    }

    // Only mark that we are free to advance in pipeline if we have everything we need to do so
    if (gotRs1 && gotRs2)
    {
        free[DECODE] = true;
        return true;
    }

    return false; // Default;
}

bool Instruction_R::execute(CPU *cpu, Scoreboard *scoreboard)
{
    // If the instruction is not decoded, we cant execute;
    if (!free[DECODE])
        return false;

    assert(free[DECODE]);
    assert(gotRs1 && gotRs2);

    // Now do the ALU operation
    printf("EXEC %d %d %d\n", opcode, rs1Value, rs2Value);
    rdValue = PerformALUOperation(opcode, rs1Value, rs2Value);

    // Mark that we are free to advance in pipeline
    free[EXECUTE] = true;

    return true;
}

bool Instruction_R::writeBack(CPU *cpu, Scoreboard *scoreboard)
{
    // If the instruction has completed the memory stage
    if (!free[MEMORY])
        return false;

    assert(free[MEMORY]);

    // Write rdValue to result register, handling scoreboard stuff
    cpu->registers[rd] = rdValue;
    rdWritten = true;

    scoreboard->setValid(rd); // mark are done with rd

    // Mark we are complete in the writeback phase
    free[WRITEBACK] = true;
    return true;
}

void Instruction_R::reset()
{

    printf("resetting GOTs for %d\n", linenum);
    for (size_t i = 0; i < STAGE_COUNT; i++)
    {
        free[i] = false;
    }
    gotRs1 = false;
    gotRs2 = false;
}