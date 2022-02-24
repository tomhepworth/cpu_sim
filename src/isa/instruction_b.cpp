#include "instruction.h"
#include "isa.h"
#include "scoreboard.h"
#include "cpu.h"

#include <iostream>

bool checkCondition(OPCODE opcode, int32_t rs1Value, int32_t rs2Value)
{
    switch (opcode)
    {
    case BEQ:
        return rs1Value == rs2Value;
    case BNE:
        return rs1Value != rs2Value;
    case BLT:
        return rs1Value < rs2Value;
    case BGE:
        return rs1Value >= rs2Value;
    default:
        return false;
        break;
    }

    // Should never happen :/
    return false;
}

Instruction_B::Instruction_B(OPCODE _opcode, REGISTER_ABI_NAME _rs1, REGISTER_ABI_NAME _rs2, int32_t _imm)
{
    opcode = _opcode;
    rs2 = _rs2;
    rs1 = _rs1;
    imm = _imm;
}

bool Instruction_B::fetch(CPU *cpu, Scoreboard *scoreboard)
{
    // Branch instruction may change the PC, so check that it's not being messed with
    if (!scoreboard->isValid(PC))
        return false;

    /* Branch will pause the pipeline to stop instructions from beyond
       the branch being fetched.  */
    scoreboard->setInvalid(PC);

    // mark fetch as completed
    free[FETCH] = true;
    return true;
}

bool Instruction_B::decode(CPU *cpu, Scoreboard *scoreboard)
{
    // If the instruction is not fetched, we cant decode
    if (!free[FETCH])
        return false;

    // Determine if we have the values of rs1 and rs2
    // Lookup in register file, if we can get it, nice
    if (scoreboard->isValid(rs1) && scoreboard->isValid(rs2))
    {
        // scoreboard->setInvalid(PC); // Set pc invalid as
        // std::cout << "DECODED BRANCH: uses regs " << rs1 << " and " << rs2 << std::endl;
        rs1Value = cpu->registers[rs1];
        rs2Value = cpu->registers[rs2];
        gotRs1 = true;
        gotRs2 = true;
    }

    // Only mark that we are free to advance in pipeline if we have everything we need to do so
    if (gotRs1 && gotRs2)
    {
        free[DECODE] = true;
        return true;
    }

    free[DECODE] = false;
    return false; // Default;
}

bool Instruction_B::execute(CPU *cpu, Scoreboard *scoreboard)
{
    // If the instruction is not decoded, we cant execute;
    if (!free[DECODE])
        return false;

    assert(gotRs1 && gotRs2);

    // Do whatever a B does in the execution stage
    std::cout << "COMPARING " << rs1Value << " with " << rs2Value << std::endl;
    if (checkCondition(opcode, rs1Value, rs2Value))
    {
        rdValue = cpu->registers[PC] + imm - 2;
        std::cout << "Taking branch " << rdValue << std::endl;
    }
    else
    {
        std::cout << "Not taking branch " << std::endl;
        rdValue = cpu->registers[PC] - 1;
    }

    // Mark that we are free to advance in pipeline
    free[EXECUTE] = true;
    return true;
}

bool Instruction_B::writeBack(CPU *cpu, Scoreboard *scoreboard)
{
    // If previous pipeline stage not completed we cant do this one
    if (!free[MEMORY])
        return false;

    // std::cout << "SETTING PC TO " << rdValue << std::endl;
    cpu->registers[PC] = rdValue - 1;

    scoreboard->setValid(PC);

    // Mark this stage as completed so instruction can advance in pipeline
    free[WRITEBACK] = true;
    return true;
}

void Instruction_B::reset()
{
    for (size_t i = 0; i < STAGE_COUNT; i++)
    {
        free[i] = false;
    }
    gotRs1 = false;
    gotRs2 = false;
}

void Instruction_B::setImm(int32_t _imm)
{
    imm = _imm;
}

int32_t Instruction_B::getImm()
{
    return imm;
}
