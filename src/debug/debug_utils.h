#ifndef DEBUG_H
#define DEBUG_H

#include <map>
#include "isa.h"
#include "parser.h"

extern bool debug;
extern bool step;

std::string getStringFromOpcode(OPCODE opcode);
std::string getStringFromRegName(REGISTER_ABI_NAME reg);

#endif