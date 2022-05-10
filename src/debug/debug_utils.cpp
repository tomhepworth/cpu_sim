#include "debug_utils.h"

bool debug = false;
bool step = false;
std::string stallReason = "";

std::map<std::string, OPCODE> opcodeStringMap = {
    {"add", ADD},
    {"sub", SUB},
    {"sll", SLL},
    {"srl", SRL},
    {"and", AND},
    {"or", OR},
    {"xor", XOR},
    {"addi", ADDI},
    {"sw", SW},
    {"lw", LW},
    {"beq", BEQ},
    {"bne", BNE},
    {"blt", BLT},
    {"bge", BGE},
    {"hlt", HLT},
    {"nop", NOP}};

std::map<std::string, REGISTER_ABI_NAME> registerStringMap = {
    {"zero", ZERO},
    {"x0", ZERO},
    {"ra", RA},
    {"sp", SP},
    {"gp", GP},
    {"tp", TP},

    {"t0", T0},
    {"t1", T1},
    {"t2", T2},
    {"fp", FP},

    {"s1", S1},

    {"a0", A0},
    {"a1", A1},
    {"a2", A2},
    {"a3", A3},
    {"a4", A4},
    {"a5", A5},
    {"a6", A6},
    {"a7", A7},

    {"s1", S1},
    {"s2", S2},
    {"s3", S3},
    {"s4", S4},
    {"s5", S5},
    {"s6", S6},
    {"s7", S7},
    {"s8", S8},
    {"s9", S9},
    {"s10", S10},
    {"s11", S11},

    {"t3", T3},
    {"t4", T4},
    {"t5", T5},
    {"t6", T6},

    {"pc", PC},

};

std::string getStringFromOpcode(OPCODE opcode)
{
    std::string ret = "UNKNOWN";
    for (auto it = opcodeStringMap.begin(); it != opcodeStringMap.end(); ++it)
        if (it->second == opcode)
            ret = it->first;

    return ret;
}

std::string getStringFromRegName(REGISTER_ABI_NAME reg)
{
    std::string ret = "UNKNOWN";
    for (auto it = registerStringMap.begin(); it != registerStringMap.end(); ++it)
        if (it->second == reg)
            ret = it->first;

    return ret;
}