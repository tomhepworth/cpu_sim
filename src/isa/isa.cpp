#include "isa.h"
#include <stdio.h>
#include "cpu.h"
#include "debug_utils.h"
#include <iostream>

int32_t registers[REG_UNUSED]; // extern in isa.h

Instruction::Instruction()
{
    opcode = NOP;
    rd = REGABI_UNUSED;
    rs1 = ZERO;
    rs2 = ZERO;
}

void Instruction::printSomething()
{
    std::cout << "Something" << std::endl;
}

// void Instruction::setImm(int32_t _imm)
// {
//     printf("Warning: setImm not implemented for base instruction class\n");
// }

// int32_t Instruction::getImm()
// {
//     printf("Warning: getImm not implemented for base instruction class\n");
//     return 0;
// }

int32_t PerformALUOperation(OPCODE opcode, int32_t PC, int32_t val1, int32_t val2, int32_t imm)
{
    int32_t result;
    switch (opcode)
    {
    case ADD:
        result = val1 + val2;
        break;
    case LW: // Loads and stores need to add offset to rs2
    case SW:
        result = val2 + imm;
        break;
    case ADDI:
        result = val1 + imm;
        break;
    case SUB:
        result = val1 - val2;
        break;
    case SLL:
        result = val1 << val2;
        break;
    case SRL:
        result = val1 >> val2;
        break;
    case AND:
        result = val1 & val2;
        break;
    case OR:
        result = val1 | val2;
        break;
    case XOR:
        result = val1 ^ val2;
        break;

    // For branching instructions, check condition and if met compute PC + imm (pc is current instructions pc value)
    case BEQ:
        if (val1 == val2)
        {
            result = PC + imm;
        }
        else
        {
            result = PC + 1;
        }

        break;
    case BNE:
        if (val1 != val2)
        {
            result = PC + imm;
        }
        else // Slightly jank that we return pc+1, maybe refactor to use a "didBranch" control signal
        {
            result = PC + 1;
        }
        break;
    case BLT:
        if (val1 < val2)
        {
            result = PC + imm;
        }
        else
        {
            result = PC + 1;
        }
        break;
    case BGE:
        if (val1 >= val2)
        {
            result = PC + imm;
        }
        else
        {
            result = PC + 1;
        }
        break;

    default:
        break;
    }

    return result;
}

int32_t PerformMemoryOperation(CPU *cpu, OPCODE opcode, int32_t address, int32_t value)
{

    int32_t res;
    switch (opcode)
    {
    case LW:
        res = cpu->memory[address];
        if (debug)
            std::cout << "-- Loaded " << res << " from address " << address << std::endl;

        break;
    case SW:
        cpu->memory[address] = value;
        res = value;

        if (debug)
            std::cout << "-- Stored " << value << " into address " << address << std::endl;
        break;
    default:
        break;
    }

    return res;
}
