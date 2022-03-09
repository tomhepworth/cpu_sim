#include "cpu.h"
#include "isa.h"
#include "debug_utils.h"

void PipelineStage::flush()
{
    empty = true;
}

void PipelineStage::setAsEmpty()
{
    empty = true;
    opcode = NOP;
}

void PipelineStage::AcceptData()
{
    empty = false;
}

bool PipelineStage::onCycle()
{
    if (empty)
        return true;

    return false;
}

FetchUnit::FetchUnit(CPU *_cpu)
{
    cpu = _cpu;
    PCValue = 0;
    empty = true;
}

DecodeUnit::DecodeUnit(CPU *_cpu)
{
    cpu = _cpu;
    PCValue = 0;
    empty = true;
    opcode = NOP;
    rd = ZERO;
    rs1Value = 0;
    rs2Value = 0;
    imm = 0;
    requiresMemoryAccess = false;
    requiresWriteBack = false;
}

ExecuteUnit::ExecuteUnit(CPU *_cpu)
{
    cpu = _cpu;
    PCValue = 0;
    empty = true;
    opcode = NOP;
    rd = ZERO;
    rs1Value = 0;
    rs2Value = 0;
    imm = 0;
    requiresMemoryAccess = false;
    requiresWriteBack = false;
}

MemoryAccessUnit::MemoryAccessUnit(CPU *_cpu)
{
    cpu = _cpu;
    PCValue = 0;
    empty = true;
    opcode = NOP;

    value = 0;
    address = 0;
    rd = ZERO;

    requiresMemoryAccess = false;
    requiresWriteBack = false;
}

WriteBackUnit::WriteBackUnit(CPU *_cpu)
{
    cpu = _cpu;
    PCValue = 0;
    empty = true;
    opcode = NOP;
}

bool FetchUnit::onCycle()
{
    // If fetch unit is empty then fetch a new instruction
    if (empty)
        return true;

    instruction = cpu->program->at(PCValue);
    opcode = instruction->opcode;

    empty = false;

    if (debug)
        std::cout << "Fetched " << PCValue << std::endl;

    return true;
}

bool DecodeUnit::onCycle()
{
    if (empty)
        return true;

    Scoreboard *sb = cpu->scoreboard;
    opcode = instruction->opcode;
    REGISTER_ABI_NAME rs1 = instruction->rs1;
    REGISTER_ABI_NAME rs2 = instruction->rs2;
    rd = instruction->rd;

    std::cout << " DU GOT RD " << getStringFromRegName(rd) << std::endl;
    imm = instruction->imm;

    requiresWriteBack = instruction->requiresWriteBack;
    requiresMemoryAccess = instruction->requiresMemoryAccess;

    if (sb->isValid(rs1) && sb->isValid(rs2))
    {
        rs1Value = cpu->registers[rs1];
        rs2Value = cpu->registers[rs2];

        if (requiresWriteBack)
            sb->setInvalid(rd);

        if (debug)
            std::cout << "Decoded instruction \"" << cpu->program->at(PCValue)->rawText << "\" reqwb:reqma = " << requiresWriteBack << ":" << requiresMemoryAccess << " Rd:" << getStringFromRegName(rd) << std::endl;

        return true;
    }
    else if (debug)
    {
        std::cout << "Couldnt decode " << PCValue << "Valid: rs1 (" << getStringFromRegName(rs1) << ") :" << sb->isValid(rs1) << "\trs2 (" << getStringFromRegName(rs2) << ") :" << sb->isValid(rs2) << std::endl;
    }

    return false;
}

bool ExecuteUnit::onCycle()
{
    if (empty)
        return true;

    result = PerformALUOperation(opcode, PCValue, rs1Value, rs2Value, imm);

    if (debug)
        std::cout << "ALU operation " << getStringFromOpcode(opcode) << " on " << rs1Value << ", " << rs2Value << ", " << imm << " with result" << result << " ....Rd:" << getStringFromRegName(rd) << std::endl;

    return true;
}

bool MemoryAccessUnit::onCycle()
{
    if (empty)
        return true;

    if (requiresMemoryAccess)
    {
        if (debug)
            std::cout << "Memory Operation on location : " << address << std::endl;

        value = PerformMemoryOperation(cpu, opcode, address, value); // Do load or store
    }
    else if (debug)
    {
        std::cout << "Memory operation not required" << std::endl;
    }

    std::cout << " MAU Rd:" << getStringFromRegName(rd) << "VAL IS " << value << std::endl;

    // If not complete return false

    return true;
}

bool WriteBackUnit::onCycle()
{
    std::cout << "CURR WBU VAL IS " << value << std::endl;

    if (empty)
        return true;

    if (opcode == HLT)
    {
        cpu->running = false;
    }

    if (requiresWriteBack)
    {
        if (debug)
            std::cout << "Writing back " << value << " into " << getStringFromRegName(rd) << std::endl;

        cpu->registers[rd] = value;
        cpu->scoreboard->setValid(rd);
    }
    else if (debug)
    {
        std::cout << "Writeback operation not required" << std::endl;
    }

    return true;
}