#include "reservationStation.hpp"

ReservationStation::ReservationStation(TAG masterTag, int32_t i)
{
    tag = masterTag.append(std::to_string(i));
    empty = true;
    source1 = "";
    source2 = "";
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
void ReservationStation::set(bool _valid, OPCODE _op, TAG _s1, int32_t _v1, TAG _s2, int32_t _v2)
{
    empty = false;
    valid = _valid;
    operation = _op;
    source1 = _s1;
    source2 = _s2;
    val1 = _v1;
    val2 = _v2;
}

void ReservationStation::clear()
{
    empty = true;
    valid = true;
    operation = NOP;
    source1 = "";
    source2 = "";
    val1 = 0;
    val2 = 0;
}

DistributedReservationStation::DistributedReservationStation(ReservationStationTable *_table, int _size, std::string _masterTag, RESERVATION_STATION_TYPE _type)
{
    rstable = _table;
    size = _size;
    masterTag = _masterTag;
    type = _type;

    for (int32_t i = 0; i < size; i++)
    {
        stations.push_back(new ReservationStation(masterTag, i));
        rstable->table.push_back(stations[i]); // Save a reference to the RS in the master table
    }

    // Store a reference to the group of reservation stations assigned to a FU.
    rstable->stationGroups.push_back(this);
}