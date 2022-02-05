#include "isa.h"

int32_t registers[REG_UNUSED]; // extern in isa.h

void CPU_Add(REGISTER_ABI_NAME rd, REGISTER_ABI_NAME rs1, REGISTER_ABI_NAME rs2)
{
    registers[rd] = registers[rs1] + registers[rs2];
}

void CPU_Sub(REGISTER_ABI_NAME rd, REGISTER_ABI_NAME rs1, REGISTER_ABI_NAME rs2)
{
    registers[rd] = registers[rs1] - registers[rs2];
}