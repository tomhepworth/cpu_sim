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
    for (int i = 0; i < STAGE_COUNT; i++)
    {
        stageIndexes[i] = -i;
    }
}

void Pipeline::Advance()
{
    for (int i = 0; i < STAGE_COUNT; i++)
    {
        if (stageIndexes[i] < (int)program.size())
            stageIndexes[i]++;
    }

    instructionBuffer[FETCH] = program[stageIndexes[FETCH]];
    instructionBuffer[DECODE] = program[stageIndexes[DECODE]];
    instructionBuffer[EXECUTE] = program[stageIndexes[EXECUTE]];
    instructionBuffer[MEMORY] = program[stageIndexes[MEMORY]];
    instructionBuffer[WRITEBACK] = program[stageIndexes[WRITEBACK]];

    if (printStages)
    {
        std::cout
            << "---------------------------------------------------------" << std::endl;
        std::cout << "STAGE INDEX: \t " << stageIndexes[FETCH] << "\t" << stageIndexes[DECODE] << "\t" << stageIndexes[EXECUTE] << "\t" << stageIndexes[MEMORY] << "\t" << stageIndexes[WRITEBACK] << std::endl;
        std::cout << "OPCODE: \t" << instructionBuffer[FETCH].opcode << "\t" << instructionBuffer[DECODE].opcode << "\t" << instructionBuffer[EXECUTE].opcode << "\t" << instructionBuffer[MEMORY].opcode << "\t" << instructionBuffer[WRITEBACK].opcode << std::endl;
    }
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
    // Advance the pipeline
    pipeline.Advance();

    // Fetch - CPU reads instructions from the address in the memory whose value is present in the program counter
    // Decode - instruction is decoded and the register file is accessed to get the values from the registers used in the instruction.
    // Execute - ALU operations are performed.
    // Memory Access - memory operands are read and written from/to the memory that is present in the instruction.
    // Write Back - computed / fetched value is written back to the register present in the instructions.
}
