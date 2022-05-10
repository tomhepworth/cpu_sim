#ifndef FU_H
#define FU_H

#include "cdb.hpp"
#include "reservationStation.hpp"
#include "reorderBuffer.hpp"
#include "isa.h"
#include "cpu.h"

int32_t TomasulosPerformMemoryOperation(int32_t *memory, OPCODE opcode, int32_t address, int32_t value);

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
    int32_t *memory;
    int32_t *physicalRegisters;

    OPCODE opcode;
    int32_t rs1Val;
    int32_t rs2Val;
    int32_t imm;
    int32_t pcValue;

    // Used in load store unit to decide whether to do a memory operation or calcualte an address
    enum LOAD_STORE_PHASE
    {
        ADDR_CALC,
        MEMORY_OPERATION
    };
    int phase;

    TomasuloFunctionalUnit(int32_t *_physicalRegisters, int32_t *_memory, CommonDataBus *_cdb, ReservationStationTable *_rst, TAG _tag, ReorderBuffer *_rob, RESERVATION_STATION_TYPE _type);
};

class LoadStoreUnit : public TomasuloFunctionalUnit
{
public:
    /*
    Note:
    For simplicity take a conservative approach to memory disambiguation.
    Just stall a load until all previous stores have committed

    do this by getting the OLDEST READY memory operation from the distributed reservation station
    */

    int32_t calculatedAddress;
    int32_t value;
    int32_t result;
    TAG resultTag;

    // Inherit constructor
    using TomasuloFunctionalUnit::TomasuloFunctionalUnit;

    /* Take from load store queue (res station), calculate address, do the load/store, pass to CDB for committing*/
    void Cycle();
    void flush();

    void print();
};

class AdderUnit : public TomasuloFunctionalUnit
{
public:
    // Inherit constructor
    using TomasuloFunctionalUnit::TomasuloFunctionalUnit;

    /*
     Take ready instruction from reservation stations, execute it, pass completed to CDB
    */
    void Cycle();
    void flush();

    void print();
};

#endif