#ifndef FU_H
#define FU_H

#include "cdb.hpp"
#include "reservationStation.hpp"
#include "cpu.h"

class TomasuloFunctionalUnit
{
public:
    CommonDataBus *cdb;
    DistributedReservationStation *reservationStations;
    ReservationStationTable *rst; // The FU's reference to the status of all reservation stations, might not be needed in the end
    TAG tag;
    RESERVATION_STATION_TYPE type;
    ReservationStation *toExecute;
    ReorderBuffer *rob;

    OPCODE opcode;
    int32_t rs1Val;
    int32_t rs2Val;
    int32_t imm;
    int32_t pcValue;

    TomasuloFunctionalUnit(CommonDataBus *_cdb, ReservationStationTable *_rst, TAG _tag, ReorderBuffer *_rob, RESERVATION_STATION_TYPE _type)
    {
        rob = _rob;
        cdb = _cdb;
        toExecute = nullptr;
        rst = _rst;
        tag = _tag;

        std::cout << "my tag is... " << tag << std::endl;
        type = _type;
        reservationStations = new DistributedReservationStation(rst, RESERVATION_STATION_BUFFER_SIZE, tag, type, cdb);

        opcode = NOP;
        rs1Val = 0;
        rs2Val = 0;
        imm = 0;
    }
};

class LoadStoreUnit : public TomasuloFunctionalUnit
{

    /*
    Note:
    For simplicity take a conservative approach to memory disambiguation.
    Just stall a load until all previous stores have committed
    */

    // Inherit constructor
    using TomasuloFunctionalUnit::TomasuloFunctionalUnit;

    /* Take from load store queue (res station), calculate address, do the load/store, pass to CDB for committing*/
    void Cycle()
    {
        toExecute = reservationStations->getNextReady();

        // Case where none are ready to execute
        if (toExecute == nullptr)
        {
            // stall
            return;
        }

        // Otherwise execute
    }
};

class AdderUnit : public TomasuloFunctionalUnit
{
public:
    // Inherit constructor
    using TomasuloFunctionalUnit::TomasuloFunctionalUnit;

    /*
     Take ready instruction from reservation stations, execute it, pass completed to CDB
    */
    void Cycle()
    {
        toExecute = reservationStations->getNextReady();

        // Case where none are ready to execute
        if (toExecute == nullptr)
        {
            // stall
            return;
        }

        // Otherwise, execute

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

    void print()
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
};

#endif