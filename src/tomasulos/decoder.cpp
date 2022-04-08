#include "decoder.hpp"

void TomasulosDecoder::Cycle()
{

    // If PC not valid then stall
    if (!registerStatusTable->getRegValid(PC))
    {
        std::cout << "Stalling!" << std::endl;
        stalled = true;
        return;
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

    bool foundFreeReservationStation = false;
    ReservationStation *rs;

    // Search reservation station for a free space with the LOAD_STORE type
    for (auto group : reservationStationTable->stationGroups)
    {
        // This logic works because we only have adder or load_store type reservation stations rigt now
        int requiredType = ADDER;

        // If current instruction requires memory access, dispatch to load store queue
        if (currentInstruction->requiresMemoryAccess)
            requiredType = LOAD_STORE;

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

        rs->set(readyToExecute, opcode, source1Tag, source1Val, source2Tag, source2Val);

        // Set destination register tag to reservation station tag, and mark invalid
        registerStatusTable->setRegTag(rd, rs->tag);
        registerStatusTable->setRegValid(rd, false);

        // For debugging:
        lastUpdatedRS = rs->tag;
    }
    else // Otherwise stall
    {
        stalled = true;
    }

    return;
}