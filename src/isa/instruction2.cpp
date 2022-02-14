#include "cpu.h"
#include <assert.h>

class Instruction2
{
private:
protected:
    bool free[STAGE_COUNT] = {false}; // Indicates if instruction is able to advance to the next phase

public:
    OPCODE opcode;

    Instruction2();

    virtual bool fetch();
    virtual bool decode();
    virtual bool execute();
    virtual bool memoryAccess();
    virtual bool writeBack();
};

Instruction2::Instruction2()
{
    free[FETCH] = true; // Can always be fetched so set this true
}

class Instruction_R : public Instruction2
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

    Instruction_R(REGISTER_ABI_NAME _rd, REGISTER_ABI_NAME _rs1, REGISTER_ABI_NAME _rs2)
    {
        rd = _rd;
        rs1 = _rs1;
        rs2 = _rs2;
    }

    bool decode()
    {
        // If the instruction is not fetched, we cant decode
        if (!free[FETCH])
            return false;

        // Determine if we have the values of rs1 and rs2
        // Lookup in register file, if we can get it, nice

        // Only mark that we are free to advance in pipeline if we have everything we need to do so
        if (gotRs1 && gotRs2)
        {
            free[DECODE] = true;
            return true;
        }

        return false; // Default;
    }

    bool execute()
    {
        // If the instruction is not decoded, we cant execute;
        if (!free[DECODE])
            return false;

        assert(gotRs1 && gotRs2);

        // Now do the ALU operation
        rdValue = PerformALUOperation(opcode, rs1Value, rs2Value);

        // Mark that we are free to advance in pipeline
        free[EXECUTE] = true;

        return true;
    }
};

class Instruction_I : public Instruction2
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

    Instruction_I(REGISTER_ABI_NAME _rd, REGISTER_ABI_NAME _rs1, int32_t _imm)
    {
        rd = _rd;
        rs1 = _rs1;
        imm = _imm;
    }

    bool decode()
    {
        // If the instruction is not fetched, we cant decode
        if (!free[FETCH])
            return false;

        // Determine if we have the values of rs1 and rs2
        // Lookup in register file, if we can get it, nice

        // Only mark that we are free to advance in pipeline if we have everything we need to do so
        if (gotRs1)
        {
            free[DECODE] = true;
            return true;
        }

        return false; // Default;
    }

    bool execute()
    {
        // If the instruction is not decoded, we cant execute;
        if (!free[DECODE])
            return false;

        assert(gotRs1);

        // Now do the ALU operation
        rdValue = PerformALUOperation(opcode, rs1Value, imm);

        // Mark that we are free to advance in pipeline
        free[EXECUTE] = true;

        return true;
    }
};

// This function models all the ALU operations as generally as I can (IE, adds are all combined)
int PerformALUOperation(OPCODE opcode, int32_t val1, int32_t val2)
{
    int32_t result;
    switch (opcode)
    {
    case ADD:
    case ADDI:
        result = val1 + val2;
        break;
    case SUB:
        result = val1 - val2;
        break;
    default:
        break;
    }

    return result;
}