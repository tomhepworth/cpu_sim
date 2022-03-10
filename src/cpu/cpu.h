#ifndef CPU_H
#define CPU_H

#include <string>
#include <iostream>

#include "isa.h"
#include "parser.h"
#include "scoreboard.h"

class Pipeline
{
public:
    bool triggerFlush;

    Pipeline();

    virtual void setCPU(CPU *_cpu){};
    virtual bool Advance(CPU *cpu, runnable_program *program, Scoreboard *scoreboard);
    virtual void flush();
};

class CPU
{
private:
public:
    int cycles;
    bool running;

    int32_t *registers;
    int32_t *memory;

    Pipeline *pipeline;
    Scoreboard *scoreboard;
    runnable_program *program;

    int memorySize;
    int speed;

    CPU();
    CPU(Pipeline *_pipeline, Scoreboard *_scoreboard, int _memorySize, int _speed);

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

// This function models all the ALU operations as generally as I can (IE, adds are all combined)
int32_t PerformALUOperation(OPCODE opcode, int32_t PC, int32_t val1, int32_t val2, int32_t imm);

// Models MemoryAccess unit function
int32_t PerformMemoryOperation(CPU *cpu, OPCODE opcode, int32_t address, int32_t value);

class PipelineStage
{
public:
    CPU *cpu; // the cpu the unit is part of
    int32_t PCValue;
    OPCODE opcode;
    bool empty;

    PipelineStage()
    {
        empty = true;
    }

    // The operation the hardware execution unit shoud perform on a cycle. Return true if operation completed
    virtual bool onCycle();
    virtual void AcceptData();
    void flush();
    void setAsEmpty();
};

class FetchUnit : public PipelineStage
{
public:
    Instruction *instruction;

    FetchUnit(){};
    FetchUnit(CPU *_cpu);

    // Get an instruction * from memory, return true if successful
    bool onCycle();

    void AcceptData(int32_t pc)
    {
        assert(empty);
        PCValue = pc;
        empty = false;
    };
};

class DecodeUnit : public PipelineStage
{
public:
    Instruction *instruction;

    REGISTER_ABI_NAME rd;
    int32_t rs1Value;
    int32_t rs2Value;
    int32_t imm;
    bool requiresWriteBack;
    bool requiresMemoryAccess;

    DecodeUnit(){};
    DecodeUnit(CPU *_cpu);

    bool onCycle();

    // Updates the rs1 rs2 and imm fields so they can be decoded on the next cycle
    bool AcceptData(int32_t pc, Instruction *_instructionToDecode)
    {
        assert(empty);
        PCValue = pc;
        instruction = _instructionToDecode;
        empty = false;

        return true;
    };
};

class ExecuteUnit : public PipelineStage
{
public:
    // TODO reservation stations before execute

    REGISTER_ABI_NAME rd;

    int32_t rs1Value;
    int32_t rs2Value;
    int32_t imm;
    int32_t result;

    bool requiresWriteBack;
    bool requiresMemoryAccess;

    ExecuteUnit(){};
    ExecuteUnit(CPU *_cpu);

    bool onCycle();

    bool AcceptData(int32_t pc, OPCODE _opcode, REGISTER_ABI_NAME _rd, int32_t _rs1Value, int32_t _rs2Value, int32_t _imm, bool reqWB, bool reqMA)
    {
        assert(empty);

        PCValue = pc;
        opcode = _opcode;
        rd = _rd;

        rs1Value = _rs1Value;
        rs2Value = _rs2Value;
        imm = _imm;

        requiresMemoryAccess = reqMA;
        requiresWriteBack = reqWB;

        empty = false;

        return true;
    };
};

class MemoryAccessUnit : public PipelineStage
{
public:
    int32_t value;
    int32_t address;
    REGISTER_ABI_NAME rd;
    bool requiresWriteBack;
    bool requiresMemoryAccess;

    MemoryAccessUnit(){};
    MemoryAccessUnit(CPU *_cpu);

    bool onCycle();
    // Accept data from previous pipeline stage
    void AcceptData(int32_t pc, OPCODE _opcode, REGISTER_ABI_NAME _rd, int32_t _address, int32_t _value, bool reqWB, bool reqMA)
    {
        PCValue = pc;
        opcode = _opcode;
        rd = _rd;
        requiresMemoryAccess = reqMA;
        requiresWriteBack = reqWB;
        address = _address;
        value = _value; // Will get overwritten in onCycle if requiresMemoryAccess is true
        empty = false;
    };
};

class WriteBackUnit : public PipelineStage
{
public:
    REGISTER_ABI_NAME rd;
    int32_t value;
    bool requiresWriteBack;

    WriteBackUnit(){};
    WriteBackUnit(CPU *_cpu);

    // commit value to registers[reg]
    bool onCycle();

    // Accept data from previous pipeline stage
    void AcceptData(int32_t pc, OPCODE _opcode, REGISTER_ABI_NAME _rd, int32_t rdValue, bool reqWB)
    {
        assert(empty); // Dont overwrite anything

        PCValue = pc;
        opcode = _opcode;
        rd = _rd;
        value = rdValue;

        requiresWriteBack = reqWB;
        empty = false;
    }
};

class ScalarPipeline : public Pipeline
{

public:
    CPU *cpu; // The cpu the pipeline is attached to

    FetchUnit fetchUnit;
    DecodeUnit decodeUnit;
    ExecuteUnit executeUnit;
    MemoryAccessUnit memoryAccessUnit;
    WriteBackUnit writebackUnit;

    ScalarPipeline(){};
    ScalarPipeline(CPU *cpu);

    bool Advance(CPU *cpu, runnable_program *program, Scoreboard *scoreboard);
    void setCPU(CPU *_cpu);
    void flush();
};

// class SuperscalarPipeline : public Pipeline
// {

// public:
//     FetchUnit *fetchUnits;
//     DecodeUnit *decodeUnits;
//     ExecuteUnit *executeUnits;
//     MemoryAccessUnit *memoryAccessUnits;
//     WriteBackUnit *writebackUnits;

//     // Initialise an n-way superscalar pipeline
//     SuperscalarPipeline();
//     SuperscalarPipeline(int n);

//     bool Advance(CPU *cpu, runnable_program *program, Scoreboard *scoreboard);
// };
#endif