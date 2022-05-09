#ifndef DEBUG_H
#define DEBUG_H

#include <map>
#include "isa.h"
#include "parser.h"

extern bool debug;
extern bool step;

#define IF_DEBUG(args) \
    {                  \
        if (debug)     \
        {              \
            args;      \
        }              \
    }

const std::string TERMINAL_RED("\033[0;31m");
const std::string TERMINAL_GREEN("\033[0;32m");
const std::string TERMINAL_YELLOW("\033[0;33m");
const std::string TERMINAL_BLUE("\033[0;34m");
const std::string TERMINAL_MAGENTA("\033[0;35m");
const std::string TERMINAL_CYAN("\033[0;36m");
const std::string TERMINAL_RESET("\033[0m");

const std::string TERMINAL_BOLD_RED("\033[1m\033[31m");
const std::string TERMINAL_BOLD_GREEN("\033[1m\033[32m");
const std::string TERMINAL_BOLD_YELLOW("\033[1m\033[33m");
const std::string TERMINAL_BOLD_BLUE("\033[1m\033[34m");
const std::string TERMINAL_BOLD_MAGENTA("\033[1m\033[35m");
const std::string TERMINAL_BOLD_CYAN("\033[1m\033[36m");

std::string getStringFromOpcode(OPCODE opcode);
std::string getStringFromRegName(REGISTER_ABI_NAME reg);

#endif