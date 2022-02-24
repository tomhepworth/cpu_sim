#ifndef CPU_H
#define CPU_H

#include <string>
#include <iostream>

#include "cpu.h"
#include "parser.h"
#include "scoreboard.h"

class Pipeline
{
private:
    int stageIndexes[STAGE_COUNT];

public:
    Pipeline();

    bool Advance(CPU *cpu, runnable_program *program, Scoreboard *scoreboard);
};

class CPU
{
private:
    Pipeline *pipeline;
    Scoreboard *scoreboard;
    runnable_program *program;

    int memorySize;
    int speed;

public:
    CPU();
    CPU(Pipeline *_pipeline, Scoreboard *_scoreboard, int _memorySize, int _speed);

    int cycles;
    bool running;
    int32_t *registers;
    int32_t *memory;

    void LoadProgram(runnable_program *prog);
    void Cycle();

    int getCycles()
    {
        return cycles;
    }

    void regDump();
    void memDump(int start, int end);

    int getMemorySize();
};

#endif