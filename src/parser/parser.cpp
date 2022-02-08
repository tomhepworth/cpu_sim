#include "parser.h"
#include "isa.h"
#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>

std::map<std::string, std::pair<OPCODE, INSTRUCTION_FORMAT>> instructionMap = {
    {"add", std::make_pair(ADD, R)},
    {"sub", std::make_pair(SUB, R)},
    {"addi", std::make_pair(ADDI, I)},
    {"hlt", std::make_pair(HLT, SPECIAL)},
    {"nop", std::make_pair(ADD, I)}}; // extern in parser.h

std::map<std::string, REGISTER_ABI_NAME> registerMap = {
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
    {"a3", A1},
    {"a4", A1},
    {"a5", A1},
    {"a6", A1},
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

}; // extern in parser.h

runnable_program parse(std::string filename)
{
    runnable_program prog;

    std::ifstream file(filename);
    std::string line;
    std::vector<std::string> words;
    std::vector<std::vector<std::string>> parsedLines;
    while (std::getline(file, line))
    {
        std::size_t prev = 0, pos;
        while ((pos = line.find_first_of(" ,\t", prev)) != std::string::npos)
        {
            if (pos > prev)
                words.push_back(line.substr(prev, pos - prev));
            prev = pos + 1;
        }
        if (prev < line.length())
            words.push_back(line.substr(prev, std::string::npos));

        parsedLines.push_back(words);
        words.clear();
    }

    for (auto parsedLine : parsedLines)
    {

        // for (auto wd : parsedLine)
        //     std::cout << wd << " | ";
        // std::cout << std::endl;

        std::pair<OPCODE, INSTRUCTION_FORMAT> decodedInstruciton = instructionMap.at(parsedLine[0]);
        OPCODE opcode = decodedInstruciton.first;
        INSTRUCTION_FORMAT format = decodedInstruciton.second;

        Instruction *i;

        switch (format)
        {
        case R:
        {
            REGISTER_ABI_NAME rd = registerMap.at(parsedLine[1]);
            REGISTER_ABI_NAME rs1 = registerMap.at(parsedLine[2]);
            REGISTER_ABI_NAME rs2 = registerMap.at(parsedLine[3]);

            i = new Instruction(opcode, format, rd, rs1, rs2);
            break;
        }
        case SPECIAL:
        {
            i = new Instruction(opcode, format);
            break;
        }
        default:
        {
            REGISTER_ABI_NAME rd = registerMap.at(parsedLine[1]);
            REGISTER_ABI_NAME rs1 = registerMap.at(parsedLine[2]);
            int32_t imm = std::atoi(parsedLine[3].c_str());
            i = new Instruction(opcode, format, rd, rs1, imm);
            break;
        }
        }
        prog.push_back(*i);
    }

    return prog;
}