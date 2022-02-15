#include "parser.h"
#include "isa.h"
#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <instruction.h>

std::map<std::string, std::pair<OPCODE, INSTRUCTION_FORMAT>> instructionMap = {
    {"add", std::make_pair(ADD, R)},
    {"sub", std::make_pair(SUB, R)},
    {"addi", std::make_pair(ADDI, I)},
    {"hlt", std::make_pair(HLT, SPECIAL)},
    {"nop", std::make_pair(NOP, SPECIAL)}}; // extern in parser.h

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

bool parse(std::string filename, runnable_program *prog)
{
    std::ifstream file(filename);
    std::string line;
    std::vector<std::string> words;
    std::vector<std::vector<std::string>> parsedLines;

    int lineNumber = 0;
    while (std::getline(file, line))
    {
        lineNumber++;
        try
        {
            // If line is comment (beginning "//" ), skip
            if (line[0] == '/' && line[1] == '/')
                continue;

            // If line is just a new line, skip
            if (line[0] == '\n' || line[0] == '\0')
                continue;

            std::size_t prev = 0, pos;
            while ((pos = line.find_first_of(" ,\t", prev)) != std::string::npos)
            {
                if (pos > prev)
                    words.push_back(line.substr(prev, pos - prev));
                prev = pos + 1;
            }
            if (prev < line.length())
                words.push_back(line.substr(prev, std::string::npos));

            // Now convert to instruction
            std::pair<OPCODE, INSTRUCTION_FORMAT> decodedInstruciton = instructionMap.at(words[0]);
            OPCODE opcode = decodedInstruciton.first;
            INSTRUCTION_FORMAT format = decodedInstruciton.second;

            Instruction *i;

            switch (format)
            {
            case R:
            {
                REGISTER_ABI_NAME rd = registerMap.at(words[1]);
                REGISTER_ABI_NAME rs1 = registerMap.at(words[2]);
                REGISTER_ABI_NAME rs2 = registerMap.at(words[3]);

                i = new Instruction_R(opcode, rd, rs1, rs2);
                break;
            }
            case I:
            {
                // TODO: Add parsing support for loading instrucitons: eg "lb t0, 8(sp)" to load t0 with (sp + 8)
                REGISTER_ABI_NAME rd = registerMap.at(words[1]);
                REGISTER_ABI_NAME rs1 = registerMap.at(words[2]);
                int32_t imm = std::atoi(words[3].c_str());
                i = new Instruction_I(opcode, rd, rs1, imm);
                break;
            }
            case S:
            {
                // Add parsing support storing instructions Eg: SW rd, offset(rs1), stores rd into memory at rs1 + offset
                REGISTER_ABI_NAME rs1 = registerMap.at(words[1]);
                REGISTER_ABI_NAME rs2 = registerMap.at(words[2]);
                int32_t imm = std::atoi(words[3].c_str());

                i = new Instruction_S(opcode, rs1, rs2, imm);
                break;
            }
            case B:
            {
                // Add parsing support storing unstructions Eg: SW rd, offset(rs1), stores rd into memory at rs1 + offset
                REGISTER_ABI_NAME rs1 = registerMap.at(words[1]);
                REGISTER_ABI_NAME rs2 = registerMap.at(words[2]);
                int32_t imm = std::atoi(words[3].c_str());

                i = new Instruction_B(opcode, rs1, rs2, imm);
                break;
            }
            case SPECIAL:
            {

                // Handle special instructions individually
                switch (opcode)
                {
                case HLT:
                    i = new Instruction_Halt();
                    break;
                case NOP: // NOP is same as default
                default:
                    i = new Instruction();
                    break;
                }
                break;
            }
            default:
                break;
            }

            prog->push_back(i);

            // cleanup
            words.clear();
        }
        catch (const std::exception &e)
        {
            std::cerr << "Parser error on line " << lineNumber << " caused by: " << e.what() << std::endl;
            return false;
        }
    }
    return true;
}