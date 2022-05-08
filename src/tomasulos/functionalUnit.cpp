#include "functionalUnit.hpp"

int32_t TomasulosPerformMemoryOperation(int32_t *memory, OPCODE opcode, int32_t address, int32_t value)
{
    int32_t res;
    switch (opcode)
    {
    case LW:
        res = memory[address];
        if (debug)
            std::cout << "-- Loaded " << res << " from address " << address << std::endl;

        break;
    case SW:
        memory[address] = value;
        res = value;

        if (debug)
            std::cout << "-- Stored " << value << " into address " << address << std::endl;
        break;
    default:
        break;
    }
    return res;
}

TomasuloFunctionalUnit::TomasuloFunctionalUnit(int32_t *_memory, CommonDataBus *_cdb, ReservationStationTable *_rst, TAG _tag, ReorderBuffer *_rob, RESERVATION_STATION_TYPE _type)
{
    rob = _rob;
    cdb = _cdb;
    toExecute = nullptr;
    rst = _rst;
    tag = _tag;
    memory = _memory;

    std::cout << "my tag is... " << tag << std::endl;
    type = _type;
    reservationStations = new DistributedReservationStation(rst, RESERVATION_STATION_BUFFER_SIZE, tag, type, cdb);

    opcode = NOP;
    rs1Val = 0;
    rs2Val = 0;
    imm = 0;

    phase = ADDR_CALC;
}

void LoadStoreUnit::Cycle()
{
    if (phase == ADDR_CALC)
    {
        /*Address calculate phase: take a new memory instruction from the reservation station
        and calculate the address it needs to access*/
        assert(reservationStations->masterTag == "LS");

        toExecute = reservationStations->getOldestReady(); // Getting from a LOAD/STORE type distributed reservation station

        // Case where none are ready to execute
        if (toExecute == nullptr)
        {
            // stall
            return;
        }

        // Mark as busy so it ccant be fetched again this cycle
        toExecute->busy = true;

        // Extract values
        opcode = toExecute->operation;
        rs1Val = toExecute->val1;
        rs2Val = toExecute->val2;
        imm = toExecute->imm;
        pcValue = toExecute->pcValue;

        resultTag = toExecute->tag;

        // Loads and stores need to add offset to rs2 to get the address
        calculatedAddress = rs2Val + imm;

        phase = MEMORY_OPERATION; // Switch phase for next time
    }
    else if (phase == MEMORY_OPERATION)
    {
        /*
        Memory operation phase...
        */
        switch (opcode)
        {
        case SW:
            // For a store we dont need to do anything other than mark the instuction as ready to be stored at calculatedAddress
            rob->updateField(opcode, resultTag, -1, calculatedAddress, rs1Val, true, true);
            break;
        case LW:
            // For a load we need to fetch the value from memory then commit the result to the ROB
            result = memory[calculatedAddress];
            rob->updateField(opcode, resultTag, result, -1, -1, true, false);
            break;
        default:
            std::cout << "ERROR UNKNOWN MEMORY OPERATION" << std::endl;
            break;
        }
        phase = ADDR_CALC;
    }
}

void LoadStoreUnit::print()
{
    // std::cout << "TAG\t" << tag << std::endl;
    if (toExecute == nullptr)
    {
        std::cout << "Empty" << std::endl;
        return;
    }

    std::cout << "RES TAG\t" << toExecute->tag << std::endl;
    std::cout << "OP\t" << getStringFromOpcode(opcode) << std::endl;
    std::cout << "RS1V\t" << rs1Val << std::endl;
    std::cout << "RS2V\t" << rs2Val << std::endl;
    std::cout << "IMM\t" << imm << std::endl;
    std::cout << "Inst PC\t" << pcValue << std::endl;
    std::cout << "ADDR\t" << calculatedAddress << std::endl;
    std::cout << "PHASE\t" << phase << std::endl;
}

void AdderUnit::Cycle()
{
    toExecute = reservationStations->getNextReady();

    // Case where none are ready to execute
    if (toExecute == nullptr)
    {
        // stall
        return;
    }

    // Otherwise, execute

    // Mark as busy so it ccant be fetched again this cycle
    toExecute->busy = true;

    // Extract values
    opcode = toExecute->operation;
    rs1Val = toExecute->val1;
    rs2Val = toExecute->val2;
    imm = toExecute->imm;
    pcValue = toExecute->pcValue;

    TAG resultTag = toExecute->tag;

    // TODO carry though instructions PC value or branches wont work
    int32_t result = PerformALUOperation(opcode, pcValue, rs1Val, rs2Val, imm);

    // update ROB
    rob->updateField(opcode, resultTag, result, -1, -1, true, false);
}

void AdderUnit::print()
{
    // std::cout << "TAG\t" << tag << std::endl;
    if (toExecute == nullptr)
        return;

    std::cout << "RES TAG\t" << toExecute->tag << std::endl;
    std::cout << "OP\t" << getStringFromOpcode(opcode) << std::endl;
    std::cout << "RS1V\t" << rs1Val << std::endl;
    std::cout << "RS2V\t" << rs2Val << std::endl;
    std::cout << "IMM\t" << imm << std::endl;
    std::cout << "Inst PC\t" << pcValue << std::endl;
}