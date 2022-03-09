#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <string>

#include "isa.h"
// #include "cpu.h"
// #include "scoreboard.h"

// forward declaration
class CPU;
class Scoreboard;

// Base instruction class
class Instruction
{
private:
protected:
public:
    int linenum;
    OPCODE opcode;
    std::string rawText;

    REGISTER_ABI_NAME rs1;
    REGISTER_ABI_NAME rs2;
    REGISTER_ABI_NAME rd;
    int32_t imm;

    bool requiresMemoryAccess;
    bool requiresWriteBack;

    std::string labelToReplace; // If the instruction contained a label that hadnt been seen yet, keep a reference of it so any relative addressing can be computed once the label is found

    Instruction();

    virtual void printSomething();
};

// Instruction class for R type instructions like add, sub etc
class Instruction_R : public Instruction
{
public:
    Instruction_R(OPCODE _opcode, REGISTER_ABI_NAME _rd, REGISTER_ABI_NAME _rs1, REGISTER_ABI_NAME _rs2);
};

// Instruction class for I type instructions like addi, lb etc
class Instruction_I : public Instruction
{
public:
    Instruction_I(OPCODE _opcode, REGISTER_ABI_NAME _rd, REGISTER_ABI_NAME _rs1, int32_t _imm);
};

// Instruction class for S type instructions like sw sb etc
class Instruction_S : public Instruction
{
public:
    Instruction_S(OPCODE _opcode, REGISTER_ABI_NAME _rs1, REGISTER_ABI_NAME _rs2, int32_t _imm);
};

// Instruction class for B type instructions like beq, bge etc
class Instruction_B : public Instruction
{
public:
    Instruction_B(OPCODE _opcode, REGISTER_ABI_NAME _rs1, REGISTER_ABI_NAME _rs2, int32_t _imm);

    int32_t getImm();
    void setImm(int32_t val);
};

class Instruction_Halt : public Instruction
{
public:
    Instruction_Halt();
};

#endif