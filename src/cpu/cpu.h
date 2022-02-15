#ifndef CPU_H
#define CPU_H

#include <string>
#include <iostream>
#include "parser.h"
#include "isa.h"
#include "scoreboard.h"

class Pipeline
{
private:
    int stageIndexes[STAGE_COUNT];

public:
    bool printStages;
    Pipeline();

    void Advance(runnable_program *program, Scoreboard *scoreboard);
};

class CPU
{
private:
    int cycles;
    Pipeline *pipeline;
    Scoreboard *scoreboard;
    runnable_program *program;

public:
    CPU();
    CPU(Pipeline *_pipeline, Scoreboard *_scoreboard);

    void LoadProgram(runnable_program *prog);
    void Cycle();

    int getCycles()
    {
        return cycles;
    }
};

#endif