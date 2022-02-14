#ifndef CPU_H
#define CPU_H

#include <string>
#include <iostream>
#include "parser.h"
#include "isa.h"

// Execution stages
enum CPU_STAGE
{
    FETCH,
    DECODE,
    EXECUTE,
    MEMORY,
    WRITEBACK,
    STAGE_COUNT // Not used, just equals number of stages
};

class Pipeline
{
private:
    runnable_program program;

    int stageIndexes[STAGE_COUNT];

    Instruction instructionBuffer[STAGE_COUNT];

public:
    bool printStages = false;
    Pipeline();
    Pipeline(runnable_program prog);

    void Advance();
};

class CPU
{
private:
    Pipeline pipeline;
    runnable_program program;

public:
    CPU();
    CPU(Pipeline _pipeline);

    void LoadProgram(runnable_program prog);
    void Cycle();
};

#endif