#ifndef DEBUG_H
#define DEBUG_H

#include <map>
#include "isa.h"
#include "parser.h"

extern bool debug;
extern bool step;

const std::string TERMINAL_RED("\033[0;31m");
const std::string TERMINAL_GREEN("\033[0;32m");
const std::string TERMINAL_YELLOW("\033[0;33m");
const std::string TERMINAL_BLUE("\033[0;34m");
const std::string TERMINAL_MAGENTA("\033[0;35m");
const std::string TERMINAL_CYAN("\033[0;36m");
const std::string TERMINAL_RESET("\033[0m");

std::string getStringFromOpcode(OPCODE opcode);
std::string getStringFromRegName(REGISTER_ABI_NAME reg);

#endif