#ifndef PARSER_H
#define PARSER_H

#include <isa.h>
#include <string>
#include <vector>
#include <map>

typedef std::vector<Instruction> program;

extern std::map<std::string, std::pair<OPCODE, INSTRUCTION_FORMAT>> instructionMap;

program parse(std::string filename);

#endif