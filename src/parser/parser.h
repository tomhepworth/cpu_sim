#ifndef PARSER_H
#define PARSER_H

#include "isa.h"
#include "instruction.h"

#include <string>
#include <vector>
#include <map>

typedef std::vector<Instruction *> runnable_program;

extern std::map<std::string, std::pair<OPCODE, INSTRUCTION_FORMAT>> instructionMap;

bool parse(std::string filename, runnable_program *prog);

#endif