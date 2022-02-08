#include "isa.h"
#include <stdio.h>
#include "main.h"

int32_t registers[REG_UNUSED]; // extern in isa.h

void CPU_Halt()
{
    printf("HALTING\n");
    running = false;
}

void CPU_Add(REGISTER_ABI_NAME rd, REGISTER_ABI_NAME rs1, REGISTER_ABI_NAME rs2)
{
    registers[rd] = registers[rs1] + registers[rs2];
}

void CPU_AddI(REGISTER_ABI_NAME rd, REGISTER_ABI_NAME rs1, int32_t imm)
{
    registers[rd] = registers[rs1] + imm;
}

void CPU_Sub(REGISTER_ABI_NAME rd, REGISTER_ABI_NAME rs1, REGISTER_ABI_NAME rs2)
{
    registers[rd] = registers[rs1] - registers[rs2];
}
