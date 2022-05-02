#ifndef CDB_H
#define CDB_H

#include "reservationStation.hpp"
#include "regStatus.hpp"

// Forward declarations
class ReservationStationTable;
class RegisterStatusTable;
typedef std::string TAG;

class CommonDataBus
{
private:
    bool free;
    TAG tag;
    int32_t value;

    ReservationStationTable *resStationTable;
    RegisterStatusTable *registerStatusTable;

public:
    CommonDataBus(){};
    CommonDataBus(ReservationStationTable *_resStationTable, RegisterStatusTable *_registerStatusTable);

    // Update reservation stations - call this after instruction has finished executing
    void broadcast(TAG t, int32_t v);
    void clear();
    TAG getTag();
    int32_t getValue();
};

#endif