#include "instruction.h"
#include "isa.h"
#include "cpu.h"
#include <iostream>

Instruction_Halt::Instruction_Halt()
{
    opcode = HLT;
}

bool Instruction_Halt::fetch(CPU *cpu, Scoreboard *scoreboard)
{
    // Branch instruction may change the PC, so check that it's not being messed with
    if (!scoreboard->isValid(PC))
        return false;

    /* Halt will pause the pipeline to stop instructions from beyond
       the branch being fetched.  */
    scoreboard->setInvalid(PC);

    // mark fetch as completed
    free[FETCH] = true;
    return true;
}

bool Instruction_Halt::writeBack(CPU *cpu, Scoreboard *scoreboard)
{
    std::cout << "--- HALTING!!! ---" << std::endl;
    cpu->running = false;
    return true;
}