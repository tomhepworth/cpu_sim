#ifndef RESERVATION_STATION_H
#define RESERVATION_STATION_H

#define RESERVATION_STATION_BUFFER_SIZE 4

#include <string>
#include <vector>
#include "isa.h"

typedef std::string TAG;

class ReservationStation;
class DistributedReservationStation;
class ReservationStationTable;

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
    TAG tag;

    bool empty;
    bool valid;
    OPCODE operation;
    TAG source1;
    TAG source2;
    int32_t val1;
    int32_t val2;
    int32_t imm;

    ReservationStation(){};

    ReservationStation(TAG masterTag, int32_t i);

    void set(bool _valid, OPCODE _op, TAG _s1, int32_t _v1, TAG _s2, int32_t _v2);
    void clear();
};

class DistributedReservationStation
{
public:
    ReservationStationTable *rstable;
    TAG masterTag;
    int32_t size;

    RESERVATION_STATION_TYPE type;

    DistributedReservationStation(){};

    DistributedReservationStation(ReservationStationTable *_table, int _size, std::string _masterTag, RESERVATION_STATION_TYPE _type);

    std::vector<ReservationStation *> stations;

    // Take an instruction and assign it to a reservation station
    void takeIn();
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
};

#endif