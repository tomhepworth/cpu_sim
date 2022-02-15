#include "instruction.h"
#include "isa.h"
#include <iostream>

Instruction_Halt::Instruction_Halt()
{
    opcode = HLT;
}

bool Instruction_Halt::writeBack(Scoreboard *scoreboard)
{
    std::cout << "--- HALTING!!! ---" << std::endl;
    running = false;
    return true;
}