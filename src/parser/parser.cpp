#include "parser.h"
#include "isa.h"
#include <stdio.h>
#include <string>

std::map<std::string, std::pair<OPCODE, INSTRUCTION_FORMAT>> instructionMap = {
    {"add", std::make_pair(ADD, R)},
    {"sub", std::make_pair(SUB, R)},
    {"nop", std::make_pair(ADD, I)}}; // extern in parser.h

program parse(std::string filename)
{
    program prog;

    // foreach line

    std::pair<OPCODE, INSTRUCTION_FORMAT>
        decodedInstruciton = instructionMap.at("add");

    OPCODE opcode = decodedInstruciton.first;
    INSTRUCTION_FORMAT format = decodedInstruciton.second;

    Instruction *i;

    // Now depending on format intepret arguments differently:
    switch (format)
    {
    case R:
        i = new Instruction(opcode, format, GP, RA, SP);
        break;

    default:
        i = new Instruction(opcode, format, SP, SP, 0xFFFF);
        break;
    }

    prog.push_back(*i);

    printf("Reading Program: %s \n", filename.c_str());

    return prog;
}