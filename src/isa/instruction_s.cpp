#include "isa.h"
#include "instruction.h"
#include "scoreboard.h"

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

    // Do whatever a store does in the execution stage

    // Mark that we are free to advance in pipeline
    free[EXECUTE] = true;

    return true;
}