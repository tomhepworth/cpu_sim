#ifndef RESERVATION_STATION_H
#define RESERVATION_STATION_H

#define RESERVATION_STATION_BUFFER_SIZE 4

#include <string>
#include <vector>
#include "cdb.hpp"
#include "isa.h"

typedef std::string TAG;

// Some forward declarations:
class ReservationStation;
class DistributedReservationStation;
class ReservationStationTable;
class CommonDataBus;

enum RESERVATION_STATION_TYPE
{
    LOAD_STORE,
    ADDER,
    LOGIC,         // not used
    FLOATING_POINT // not used
};

class ReservationStation
{
public:
    CommonDataBus *cdb;
    TAG tag;

    bool empty;
    bool valid;
    bool busy;
    OPCODE operation;
    TAG source1;
    TAG source2;
    int32_t val1;
    int32_t val2;
    int32_t imm;
    int32_t robIndex;
    int32_t pcValue;
    int32_t insertionCycle; // The cycle the instruction was inserted into a reservation station on - used to determine its age

    ReservationStation(){};

    ReservationStation(TAG masterTag, int32_t i, CommonDataBus *_cdb);

    void set(bool _valid, OPCODE _op, TAG _s1, int32_t _v1, TAG _s2, int32_t _v2, int32_t _imm, int32_t _pc, int32_t _rob_index, int32_t _insertionCycle);
    void clear();

    /*
    On cycle each reservation station should listen on the CDB for any tags it needs, update fields accordingly
    */
    void Cycle();
};

class DistributedReservationStation
{
public:
    ReservationStationTable *rstable;
    TAG masterTag;
    int32_t size;

    RESERVATION_STATION_TYPE type;

    DistributedReservationStation(){};

    DistributedReservationStation(ReservationStationTable *_table, int _size, std::string _masterTag, RESERVATION_STATION_TYPE _type, CommonDataBus *_cdb);

    std::vector<ReservationStation *> stations;

    // Get a valid RS to give to execute in a functional unit
    ReservationStation *getNextReady();
    ReservationStation *getOldestReady();
};

class ReservationStationTable
{
public:
    // The table of all reservation stations
    std::vector<ReservationStation *> table;

    // each group of res stations is assigned to a functional unit
    std::vector<DistributedReservationStation *> stationGroups;

    ReservationStationTable() {}

    ReservationStation *findByTag(TAG tag);

    // True if all reservation stations are marked as empty
    bool allStationsEmpty();

    void print();
};

#endif