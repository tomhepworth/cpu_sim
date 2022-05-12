#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <string.h>
#include <sstream>

#include "parser.h"
#include "instruction.h"
#include "debug_utils.h"

std::map<std::string, std::pair<OPCODE, INSTRUCTION_FORMAT>> instructionMap = {
    {"add", std::make_pair(ADD, R)},
    {"sub", std::make_pair(SUB, R)},
    {"sll", std::make_pair(SLL, R)},
    {"srl", std::make_pair(SRL, R)},
    {"and", std::make_pair(AND, R)},
    {"or", std::make_pair(OR, R)},
    {"xor", std::make_pair(XOR, R)},
    {"addi", std::make_pair(ADDI, I)},
    {"sw", std::make_pair(SW, S)},
    {"lw", std::make_pair(LW, I)},
    {"beq", std::make_pair(BEQ, B)},
    {"bne", std::make_pair(BNE, B)},
    {"blt", std::make_pair(BLT, B)},
    {"bge", std::make_pair(BGE, B)},
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

std::map<std::string, int> labelTable;
std::map<std::string, int> variableTable;

bool stringIsNumber(std::string line)
{
    char *p;
    strtol(line.c_str(), &p, 10);
    return *p == 0;
}

bool parseImmediateValue(int32_t *imm, std::string imm_str, int instructionNumber)
{
    bool ret = false;
    if (stringIsNumber(imm_str.c_str()))
    {
        *imm = std::atoi(imm_str.c_str());
    }
    else if (imm_str.at(0) == '.') // Variable
    {
        // Wont have seen any variables yet, so save instruction number in int slot
        variableTable.insert(std::make_pair(imm_str, instructionNumber));
        *imm = instructionNumber;
    }
    else
    {
        // Using a label, look it up in labelTable
        std::map<std::string, int>::iterator it = labelTable.find(imm_str.c_str());

        // Found in the tabel
        if (it != labelTable.end())
        {
            *imm = it->second - instructionNumber; // Set immediate value to relative address
            ret = true;
        }
        else // Not found the label, keep a reference of it so that we can do the relative addressing once we have found it
        {
            *imm = instructionNumber; // Store instruction numeber in imm so that it can be used to calculate the relative address when the label is found
        }
    }
    // std::cout << "imm for \"" << imm_str << "\" on Instruction-line " << instructionNumber << " is " << *imm << std::endl;
    return ret;
}

bool parse(std::string filename, runnable_program *prog, std::vector<int32_t> *data)
{
    std::ifstream file(filename);
    std::string line;
    std::vector<std::string> words;
    std::vector<std::vector<std::string>> parsedLines;

    if (!file.good())
    {
        std::cout << TERMINAL_RED << "Please provide a valid filepath." << std::endl;
        return false;
    }

    int lineNumber = 0;
    int instructionNumber = 0;
    bool IN_DATA_SECTION; // Once data section reached we cant leave it
    while (std::getline(file, line))
    {
        if (strcmp(line.c_str(), ".data") == 0)
        {
            IN_DATA_SECTION = true;
            continue; // continue to skip "<data>" line
        }

        if (!IN_DATA_SECTION)
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

                // Split string on delimiters " " "," "\t"
                std::size_t prev = 0, pos;
                while ((pos = line.find_first_of(" ,\t", prev)) != std::string::npos)
                {
                    if (pos > prev)
                        words.push_back(line.substr(prev, pos - prev));
                    prev = pos + 1;
                }
                if (prev < line.length())
                    words.push_back(line.substr(prev, std::string::npos));

                // Parse:
                if (words[0].at(words[0].size() - 1) == ':') // If line is a label, parse it here
                {
                    std::string labelName = words[0].substr(0, words[0].size() - 1); // Trim off colon eg "label:" -> "label"
                    labelTable.insert(std::make_pair(labelName, instructionNumber)); // Add the address to the map
                    // std::cout << "label " << labelName << "   in#" << instructionNumber << std::endl;
                }
                else // If line is an insruction parse it here
                {

                    // Convert to instruction
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
                        REGISTER_ABI_NAME rd;
                        REGISTER_ABI_NAME rs1;
                        int32_t imm;
                        bool setImm = false;
                        if (words.size() == 3) // using "sw rs1, offset(rs2)" syntax
                        {
                            std::string offsetString = "";
                            std::string rs1String = "";
                            std::string s = words[2];
                            int index = 0;
                            while (s[index] != '(') // Go until we see a bracket
                            {
                                offsetString += s[index];
                                index++;
                            }
                            index++;                // skip the bracket
                            while (s[index] != ')') // Go until we see a close bracket
                            {
                                rs1String += s[index];
                                index++;
                            }
                            rd = registerMap.at(words[1]);
                            rs1 = registerMap.at(rs1String);
                            setImm = parseImmediateValue(&imm, offsetString.c_str(), instructionNumber);
                        }
                        else
                        {
                            rd = registerMap.at(words[1]);
                            rs1 = registerMap.at(words[2]);
                            setImm = parseImmediateValue(&imm, words[3].c_str(), instructionNumber);
                        }

                        i = new Instruction_I(opcode, rd, rs1, imm);
                        if (!setImm)
                            i->labelToReplace = words[3].c_str();
                        break;
                    }
                    case S:
                    {
                        REGISTER_ABI_NAME rs1;
                        REGISTER_ABI_NAME rs2;
                        int32_t imm;
                        bool setImm = false;
                        // Add parsing support storing instructions Eg: SW rs1, offset(rs2) stores rs1 into memory at rs2 + offset
                        if (words.size() == 3) // using "sw rs1, offset(rs2)" syntax
                        {
                            std::string offsetString = "";
                            std::string rs2String = "";
                            std::string s = words[2];
                            int index = 0;
                            while (s[index] != '(') // Go until we see a bracket
                            {
                                offsetString += s[index];
                                index++;
                            }
                            index++;                // skip the bracket
                            while (s[index] != ')') // Go until we see a close bracket
                            {
                                rs2String += s[index];
                                index++;
                            }

                            rs1 = registerMap.at(words[1]);
                            rs2 = registerMap.at(rs2String);
                            setImm = parseImmediateValue(&imm, offsetString.c_str(), instructionNumber);
                        }
                        else
                        {
                            rs1 = registerMap.at(words[1]);
                            rs2 = registerMap.at(words[2]);
                            setImm = parseImmediateValue(&imm, words[3].c_str(), instructionNumber);
                        }

                        i = new Instruction_S(opcode, rs1, rs2, imm);
                        if (!setImm)
                            i->labelToReplace = words[3].c_str();
                        break;
                    }
                    case B:
                    {
                        REGISTER_ABI_NAME rs1 = registerMap.at(words[1]);
                        REGISTER_ABI_NAME rs2 = registerMap.at(words[2]);
                        int32_t imm;

                        bool setImm = parseImmediateValue(&imm, words[3], instructionNumber);

                        // std::cout << "Branch at: " << lineNumber << " " << instructionNumber << std::endl;

                        i = new Instruction_B(opcode, rs1, rs2, imm);
                        if (!setImm)
                            i->labelToReplace = words[3].c_str();
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

                    i->rawText = line;
                    i->linenum = lineNumber;
                    prog->push_back(i);
                    instructionNumber++;
                }

                // cleanup
                words.clear();
            }
            catch (const std::exception &e)
            {
                std::cerr << "Parser error on line " << lineNumber << " caused by: " << e.what() << std::endl;
                return false;
            }
        }
        else
        {
            // Data section parsing
            std::stringstream ss(line);
            std::string item;

            // If line is comment (beginning "//" ), skip
            if (line[0] == '/' && line[1] == '/')
                continue;

            // If line is just a new line, skip
            if (line[0] == '\n' || line[0] == '\0')
                continue;

            while (std::getline(ss, item, ' '))
            {
                if (item.at(0) == '.') // if
                {
                    int memoryLocation = data->size();
                    variableTable.find(item)->second = memoryLocation;
                }
                else
                {
                    data->push_back((int32_t)strtol(item.c_str(), NULL, 0));
                }
            }
        }
    }

    /*  Now iterate over any instructions that used labels that weren't found
        at the time they were parsed, and update them
     */
    for (auto &&i : *prog)
    {
        // std::cout << "casting: " << i->labelToReplace << std::endl;
        auto x = dynamic_cast<Instruction_B *>(i); // Will return nullptr if i isn't an Instruction_B
        if (x != nullptr)
        {
            if (x->labelToReplace != "" && x->labelToReplace.at(0) != '.')
            {
                // std::cout << "CASTED: " << x->labelToReplace << std::endl;
                int32_t instructionNumber = x->getImm();
                std::map<std::string, int>::iterator it = labelTable.find(x->labelToReplace.c_str());

                // Found in the table
                if (it != labelTable.end())
                {
                    // std::cout << "Updating label" << x->labelToReplace << std::endl;
                    x->setImm(it->second - instructionNumber); // Set immediate value to relative address
                }
                else // Not found the label, keep a reference of it so that we can do the relative addressing once we have found it
                {
                    std::cout << "REFERENCE TO UNKNOWN LABEL LABEL: " << x->labelToReplace << std::endl;
                }
            }

            // std::cout << "ptr: " << x << std::endl;
        }
    }

    // Do another pass and update variables with memory addresses
    for (auto &&i : *prog)
    {
        if (i->labelToReplace == "")
            continue;

        if (i->labelToReplace.at(0) == '.')
        {
            i->imm = variableTable.find(i->labelToReplace)->second;
            std::cout << "SET INSTRUCTION " << i->rawText << " IMM TO " << i->imm;
        }
    }

    return true;
}