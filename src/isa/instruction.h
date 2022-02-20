#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include "main.h"
#include "isa.h"
#include "scoreboard.h"
#include <string>

// Base instruction class
class Instruction
{
private:
protected:
public:
    OPCODE opcode;

    std::string labelToReplace; // If the instruction contained a label that hadnt been seen yet, keep a reference off it so any relative addressing can be computed once the label is found

    bool free[STAGE_COUNT] = {false}; // Indicates if instruction is able to advance to the next phase

    Instruction();

    virtual bool fetch(Scoreboard *scoreboard);
    virtual bool decode(Scoreboard *scoreboard);
    virtual bool execute(Scoreboard *scoreboard);
    virtual bool memoryAccess(Scoreboard *scoreboard);
    virtual bool writeBack(Scoreboard *scoreboard);
    virtual void reset();
};

// Instruction class for R type instructions like add, sub etc
class Instruction_R : public Instruction
{
protected:
    int32_t rs1Value;
    int32_t rs2Value;
    int32_t rdValue;

    bool gotRs1 = false;
    bool gotRs2 = false;
    bool rdWritten = false;

public:
    REGISTER_ABI_NAME rs1;
    REGISTER_ABI_NAME rs2;
    REGISTER_ABI_NAME rd;

    Instruction_R(OPCODE _opcode, REGISTER_ABI_NAME _rd, REGISTER_ABI_NAME _rs1, REGISTER_ABI_NAME _rs2);

    bool decode(Scoreboard *scoreboard);

    bool execute(Scoreboard *scoreboard);

    // Dont think R-type does anything here
    // bool memory();
    bool writeBack(Scoreboard *scoreboard);

    void reset();
};

// Instruction class for I type instructions like addi, lb etc
class Instruction_I : public Instruction
{
protected:
    int32_t rs1Value;
    int32_t imm;
    int32_t rdValue;

    bool gotRs1 = false;
    bool rdWritten = false;

public:
    REGISTER_ABI_NAME rs1;
    REGISTER_ABI_NAME rs2;
    REGISTER_ABI_NAME rd;

    Instruction_I(OPCODE _opcode, REGISTER_ABI_NAME _rd, REGISTER_ABI_NAME _rs1, int32_t _imm);

    bool decode(Scoreboard *scoreboard);
    bool execute(Scoreboard *scoreboard);
    bool memoryAccess(Scoreboard *scoreboard);
    bool writeBack(Scoreboard *scoreboard);
    void reset();
};

// Instruction class for S type instructions like sw sb etc
class Instruction_S : public Instruction
{
protected:
    int32_t rs1Value;
    int32_t rs2Value;
    int32_t imm;
    int32_t rdValue;

    bool gotRs1 = false;
    bool gotRs2 = false;
    bool rdWritten = false;

public:
    REGISTER_ABI_NAME rs1;
    REGISTER_ABI_NAME rs2;
    REGISTER_ABI_NAME rd;

    Instruction_S(OPCODE _opcode, REGISTER_ABI_NAME _rs1, REGISTER_ABI_NAME _rs2, int32_t _imm);
    bool decode(Scoreboard *scoreboard);
    bool execute(Scoreboard *scoreboard);
    bool memoryAccess(Scoreboard *scoreboard);
    void reset();
};

// Instruction class for B type instructions like beq, bge etc
class Instruction_B : public Instruction
{
protected:
    int32_t rs1Value;
    int32_t rs2Value;
    int32_t imm;
    int32_t rdValue;

    bool gotRs1 = false;
    bool gotRs2 = false;
    bool rdWritten = false;

public:
    REGISTER_ABI_NAME rs1;
    REGISTER_ABI_NAME rs2;
    REGISTER_ABI_NAME rd;

    Instruction_B(OPCODE _opcode, REGISTER_ABI_NAME _rs1, REGISTER_ABI_NAME _rs2, int32_t _imm);

    bool fetch(Scoreboard *scoreboard);
    bool decode(Scoreboard *scoreboard);
    bool execute(Scoreboard *scoreboard);
    bool writeBack(Scoreboard *scoreboard);
    void reset();

    void setImm(int32_t _imm);
    int32_t getImm();

    // bool memory();
    // bool writeBack();
};

class Instruction_Halt : public Instruction
{

public:
    Instruction_Halt();

    // HLT instruction will get all the way to the end of the pipeline then mark running as false
    bool writeBack(Scoreboard *scoreboard);
};

#endif