#include "reservationStation.hpp"
#include "debug_utils.h"
#include <iostream>

ReservationStation::ReservationStation(TAG masterTag, int32_t i, CommonDataBus *_cdb)
{
    cdb = _cdb;
    tag = masterTag.append(std::to_string(i));
    empty = true;
    source1 = "";
    source2 = "";
    valid = false;
    robIndex = -1;
}

ReservationStation *ReservationStationTable::findByTag(TAG tag)
{
    ReservationStation *ret = nullptr;
    for (auto group : this->stationGroups)
    {
        for (auto rs : group->stations)
        {
            if (rs->tag == tag)
            {
                ret = rs;
            }
        }
    }

    return ret;
}

void ReservationStationTable::print()
{
    std::cout << "TAG\tEMPTY\tVALID\tOP\tS1\tV1\tS2\tV2\tIMM\tROBINDEZ" << std::endl;
    for (ReservationStation *rs : table)
    {
        std::cout << rs->tag << "\t" << rs->empty << "\t" << rs->valid << "\t" << getStringFromOpcode(rs->operation) << "\t" << rs->source1 << "\t" << rs->val1 << "\t" << rs->source2 << "\t" << rs->val2 << "\t" << rs->imm << "\t" << rs->robIndex << std::endl;
    }
}

void ReservationStation::set(bool _valid, OPCODE _op, TAG _s1, int32_t _v1, TAG _s2, int32_t _v2, int32_t _imm, int32_t _pc, int32_t _rob_index)
{
    std::cout << "SETTING RS OF TAG " << tag << std::endl;
    empty = false;
    valid = _valid;
    operation = _op;
    source1 = _s1;
    source2 = _s2;
    val1 = _v1;
    val2 = _v2;
    imm = _imm;
    robIndex = _rob_index;
    pcValue = _pc;
}

void ReservationStation::clear()
{
    std::cout << "clear was called on RS" << std::endl;
    empty = true;
    valid = false; // empty does not mean ready to execute
    operation = NOP;
    source1 = "";
    source2 = "";
    val1 = 0;
    val2 = 0;
    robIndex = -1;
}

void ReservationStation::Cycle()
{
    // See CommonDataBus::broadcast()

    TAG currentCDBTag = cdb->getTag();

    if (currentCDBTag == source1)
    {
        val1 = cdb->getValue();
        source1 = "";
    }

    if (currentCDBTag == source2)
    {
        val2 = cdb->getValue();
        source2 = "";
    }

    if (source1 == "" && source2 == "")
    {
        valid = true; // Mark ready to execute
    }
}

DistributedReservationStation::DistributedReservationStation(ReservationStationTable *_table, int _size, std::string _masterTag, RESERVATION_STATION_TYPE _type, CommonDataBus *_cdb)
{
    rstable = _table;
    size = _size;
    masterTag = _masterTag;
    type = _type;

    for (int32_t i = 0; i < size; i++)
    {
        stations.push_back(new ReservationStation(masterTag, i, _cdb));
        rstable->table.push_back(stations[i]); // Save a reference to the RS in the master table
    }

    // Store a reference to the group of reservation stations assigned to a FU.
    rstable->stationGroups.push_back(this);
}

ReservationStation *DistributedReservationStation::getNextReady()
{
    ReservationStation *ret = nullptr;

    for (ReservationStation *rs : stations)
    {
        if (rs->valid && !rs->empty)
        {
            assert(rs->source1 == "" && rs->source2 == "");
            ret = rs;
            break;
        }
    }

    return ret;
}