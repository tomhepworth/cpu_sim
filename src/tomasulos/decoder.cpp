#include "decoder.hpp"
#include "debug_utils.h"
#include "reorderBuffer.hpp"

RESERVATION_STATION_TYPE getReservationStationTypeFromOpode(OPCODE opcode)
{
    switch (opcode)
    {
    case LW:
    case SW:
        return LOAD_STORE;
        break;
    default:
        return ADDER;
        break;
    }
}

bool TomasulosDecoder::Cycle(int32_t cpuCycle)
{
    // At start of cycle assume not stalled
    stalled = false;

    // If PC not valid then stall
    if (!registerStatusTable->getRegValid(PC))
    {
        std::cout << "Stalling!" << std::endl;
        stalled = true;
        return stalled;
    }

    // If not stalled then take a new instruction from instruction memory
    // Do this on every cycle as PC shouldnt increase if the decoder stalls
    int32_t pcValue = registerStatusTable->getRegValue(PC);
    currentInstruction = program->at(pcValue);

    //"Decode"
    opcode = currentInstruction->opcode;
    rs1 = currentInstruction->rs1;
    rs2 = currentInstruction->rs2;
    rd = currentInstruction->rd;
    imm = currentInstruction->imm;
    instructionPC = pcValue;

    // Special case, dont try and put a NOP instruction in a reservation station, just do nothing
    if (opcode == NOP)
    {
        stalled = true;
        return stalled;
    }

    // Special case, if we are halted return, dont send HLT through the pipeline
    if (opcode == HLT)
    {
        // In the cycle, If ROB is empty, no reservation stations are busy, and we saw a halt...
        // Then we didnt see a halt through speculative execution so it is safe to exit the program
        stalled = true;
        return stalled;
    }

    bool foundFreeReservationStation = false;
    ReservationStation *rs;

    // Search reservation station for a free space
    for (auto group : reservationStationTable->stationGroups)
    {
        // Get the required type of reservation station
        int requiredType = getReservationStationTypeFromOpode(opcode);

        if (group->type == requiredType)
        {
            // Search within group for a free space
            for (auto currentRS : group->stations)
            {
                if (currentRS->empty)
                {
                    rs = currentRS;
                    rs->empty = false;
                    foundFreeReservationStation = true;

                    std::cout << "Found empty RS with tag: " << rs->tag << std::endl;
                }
                else
                {
                    std::cout << currentRS->tag << " not empty" << std::endl;
                }

                if (foundFreeReservationStation)
                    break;
            }

            if (foundFreeReservationStation)
                break;
        }
    }

    // If a reservation sation was found, populate it
    if (foundFreeReservationStation)
    {

        // Look up sources in reg status table, populate values if sources are valid
        bool source1Valid = registerStatusTable->getRegValid(rs1);
        bool source2Valid = registerStatusTable->getRegValid(rs2);
        int32_t source1Val = 0, source2Val = 0;
        TAG source1Tag = "", source2Tag = "";
        bool readyToExecute = false;

        // If operands are valid inset with values + no tags and mark as ready
        // Otherwise set tags to the tag of the source register from the register stats table
        if (source1Valid)
        {
            source1Val = registerStatusTable->getRegValue(rs1);
            std::cout << "source1val: " << source1Val << std::endl;
            source1Tag = "";
        }
        else
        {
            source1Tag = registerStatusTable->getRegTag(rs1);

            assert(!registerStatusTable->getRegValid(rs1));
            assert(source1Tag != "");
        }

        if (source2Valid)
        {
            source2Val = registerStatusTable->getRegValue(rs2);
            source2Tag = "";
        }
        else
        {
            source2Tag = registerStatusTable->getRegTag(rs2);

            assert(!registerStatusTable->getRegValid(rs2));
            assert(source2Tag != "");
        }

        if (source1Valid && source2Valid)
            readyToExecute = true;

        // Push into ReorderBuffer
        ReorderBufferEntry *robEntry = new ReorderBufferEntry(opcode, rs->tag, -1, -1, -1, pcValue);
        int32_t robReference = rob->push(robEntry);
        std::cout << "ROB REF IS " << robReference << std::endl;
        if (robReference == -1) // ROB was full, stall
        {
            stalled = true;
            return stalled;
        }

        // Set reservation station values
        rs->set(readyToExecute, opcode, source1Tag, source1Val, source2Tag, source2Val, imm, instructionPC, robReference, cpuCycle);

        std::cout << "ROB SET IS " << rs->robIndex << std::endl;

        // Set destination register tag to reservation station tag, and mark invalid
        // but as a special case dont do this for x0 - x0 should never change
        if (rd != ZERO)
        {
            registerStatusTable->setRegTag(rd, rs->tag);
            registerStatusTable->setRegValid(rd, false);
        }

        // For debugging:
        lastUpdatedRS = rs->tag;
    }
    else // Otherwise stall
    {
        stalled = true;
    }

    return stalled;
}

void TomasulosDecoder::print()
{
    std::cout << "OPCODE:\t" << getStringFromOpcode(opcode) << std::endl;
    std::cout << "RS1:\t" << getStringFromRegName(rs1) << std::endl;
    std::cout << "RS2:\t" << getStringFromRegName(rs2) << std::endl;
    std::cout << "RD:\t" << getStringFromRegName(rd) << std::endl;
    std::cout << "IMM:\t" << imm << std::endl;
    std::cout << "Inst PC:\t" << instructionPC << std::endl;
    std::cout << "STALLED?:\t" << stalled << std::endl;
}
