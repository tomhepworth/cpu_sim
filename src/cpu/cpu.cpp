#include "cpu.h"

#include <iostream>
#include <stdlib.h>

Pipeline::Pipeline()
{
}

Pipeline::Pipeline(runnable_program prog)
{
    program = prog;

    // Initialise stages to 0, -1, -2 ... -(STAGE_COUNT - 1)
    for (int i = 0; i < Pipeline::STAGE_COUNT; i++)
    {
        stageIndexes[i] = -i;
    }
}

void Pipeline::Advance()
{
    for (int i = 0; i < Pipeline::STAGE_COUNT; i++)
    {
        if (stageIndexes[i] < (int)program.size())
            stageIndexes[i]++;
    }

    instructionBuffer[FETCH] = program[stageIndexes[FETCH]];
    instructionBuffer[DECODE] = program[stageIndexes[DECODE]];
    instructionBuffer[EXECUTE] = program[stageIndexes[EXECUTE]];
    instructionBuffer[MEMORY] = program[stageIndexes[MEMORY]];
    instructionBuffer[WRITEBACK] = program[stageIndexes[WRITEBACK]];

    std::cout
        << "----------------------------------------------" << std::endl;
    std::cout << stageIndexes[FETCH] << "\t" << stageIndexes[DECODE] << "\t" << stageIndexes[EXECUTE] << "\t" << stageIndexes[MEMORY] << "\t" << stageIndexes[WRITEBACK] << std::endl;
    std::cout << instructionBuffer[FETCH].opcode << "\t" << instructionBuffer[DECODE].opcode << "\t" << instructionBuffer[EXECUTE].opcode << "\t" << instructionBuffer[MEMORY].opcode << "\t" << instructionBuffer[WRITEBACK].opcode << std::endl;
}

CPU::CPU()
{
    pipeline = Pipeline();
}

CPU::CPU(Pipeline _pipeline)
{
    pipeline = _pipeline;
}

void CPU::LoadProgram(runnable_program prog)
{
    program = prog;

    if (program.empty())
    {
        std::cout << "Empty program! Exiting" << std::endl;
        exit(EXIT_FAILURE);
    }
    else
    {
        std::cout << "Loaded program!" << std::endl;
    }
}

void CPU::Cycle()
{
    pipeline.Advance();
}
