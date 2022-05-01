#ifndef CDB_H
#define CDB_H

#include "reservationStation.hpp"
#include "regStatus.hpp"

// // Some forward declarations:
// class ReservationStationTable;
// class RegisterStatusTable;
// class RegisterStatusEntry;

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

    CommonDataBus(ReservationStationTable *_resStationTable, RegisterStatusTable *_registerStatusTable)
    {
        free = true;
        tag = "";
        value = 0;
        resStationTable = _resStationTable;
        registerStatusTable = _registerStatusTable;
    };

    // Update reservation stations - call this after instruction has finished executing
    void broadcast(TAG t, int32_t v)
    {
        /*
        Note, this way of doing thigs means results are always immediately committed.
        */

        std::cout << "BROADCASTING:: " << t << " " << v << std::endl;

        free = false;
        tag = t;
        value = v;

        // Update the reservation stations by setting every value where current tag is found
        for (ReservationStation *rs : resStationTable->table)
        {
            if (tag == rs->source1)
            {
                rs->val1 = value;
                rs->source1 = "";
            }

            if (tag == rs->source2)
            {
                rs->val2 = value;
                rs->source2 = "";
            }

            if (rs->source1 == "" && rs->source2 == "")
            {
                rs->valid = true; // Mark ready to execute
            }
        }

        // Update the register values in the register status table
        for (RegisterStatusEntry *rse : registerStatusTable->entries)
        {
            if (rse->tag == tag)
            {
                rse->value = value;
                rse->valid = true;
                rse->tag = "";
            }
        }
    };

    void clear()
    {
        free = true;
        tag = "";
        value = 0;
    };

    TAG getTag()
    {
        return tag;
    };

    int32_t getValue()
    {
        return value;
    };
};

#endif