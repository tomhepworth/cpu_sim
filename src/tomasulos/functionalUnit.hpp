#ifndef FU_H
#define FU_H

#include "tomasulo.hpp"
#include "reservationStation.hpp"

class TomasuloFunctionalUnit
{
public:
    DistributedReservationStation *reservationStations;
    ReservationStationTable *rst; // The FU's reference to the status of all reservation stations, might not be needed in the end
    TAG tag;
    RESERVATION_STATION_TYPE type;

    TomasuloFunctionalUnit(ReservationStationTable *_rst, TAG _tag, RESERVATION_STATION_TYPE _type)
    {
        rst = _rst;
        tag = _tag;
        type = _type;
        reservationStations = new DistributedReservationStation(rst, RESERVATION_STATION_BUFFER_SIZE, tag, type);
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
    void Cycle();
};

class AdderUnit : public TomasuloFunctionalUnit
{
    // Inherit constructor
    using TomasuloFunctionalUnit::TomasuloFunctionalUnit;

    /*
     Take ready instruction from reservation stations, execute it, pass completed to CDB
    */
    void Cycle()
    {
    }
};

#endif