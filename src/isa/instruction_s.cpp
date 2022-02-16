#include "isa.h"
#include "instruction.h"
#include "scoreboard.h"
#include "stdio.h"

Instruction_S::Instruction_S(OPCODE _opcode, REGISTER_ABI_NAME _rs1, REGISTER_ABI_NAME _rs2, int32_t _imm)
{
    opcode = _opcode;
    rs1 = _rs2;
    rs1 = _rs1;
    imm = _imm;
}

bool Instruction_S::decode(Scoreboard *scoreboard)
{
    // If the instruction is not fetched, we cant decode
    if (!free[FETCH])
        return false;

    // Determine if we have the values of rs1 and rs2
    // Lookup in register file, if we can get it, nice
    if (scoreboard->isValid(rs1) && scoreboard->isValid(rs2))
    {
        // Set RD to invalid so that it is reserved for this instruction to use
        rs1Value = registers[rs1];
        rs2Value = registers[rs2];

        gotRs1 = true;
        gotRs2 = true;
    }

    // Only mark that we are free to advance in pipeline if we have everything we need to do so
    if (gotRs1 && gotRs2)
    {
        free[DECODE] = true;
        return true;
    }

    return false; // Default;
}

bool Instruction_S::execute(Scoreboard *scoreboard)
{
    // If the instruction is not decoded, we cant execute;
    if (!free[DECODE])
        return false;

    assert(gotRs1 && gotRs2);

    // Add offset to rs1; TODO: maybe rename to "accumulator" something, since it's not actually used for rd
    printf("OPCODE : %d, RS2 %d, IMM %d\n", opcode, rs2, imm);
    rdValue = PerformALUOperation(opcode, rs2Value, imm);

    // Mark that we are free to advance in pipeline
    free[EXECUTE] = true;

    return true;
}

bool Instruction_S::memoryAccess(Scoreboard *scoreboard)
{
    // If previous pipeline stage not completed we cant do this one
    if (!free[EXECUTE])
        return false;

    // Store rs1 value into memory at rs2 + imm
    printf("storing %d in mem[%d]\n", rs1Value, rdValue);
    memory[rdValue] = rs1Value;

    // Mark this stage as completed so instruction can advance in pipeline
    free[MEMORY] = true;
    return true;
}