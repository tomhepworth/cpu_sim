#include "cdb.hpp"

CommonDataBus::CommonDataBus(ReservationStationTable *_resStationTable, RegisterStatusTable *_registerStatusTable)
{
    free = true;
    tag = "";
    value = 0;
    resStationTable = _resStationTable;
    registerStatusTable = _registerStatusTable;
};

void CommonDataBus::broadcast(TAG t, int32_t v)
{
    /*
    Note, this way of doing thigs means results are always immediately committed.
    */

    // IF_DEBUG(std::cout << "BROADCASTING:: " << t << " " << v << std::endl);

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

        // Clear the reservation station and mark as empty when we broadcast with ITS OWN tag
        if (tag == rs->tag)
        {
            rs->clear();
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
void CommonDataBus::clear()
{
    free = true;
    tag = "";
    value = 0;
};

TAG CommonDataBus::getTag()
{
    return tag;
};

int32_t CommonDataBus::getValue()
{
    return value;
};