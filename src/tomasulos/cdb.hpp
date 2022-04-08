#ifndef CDB_H
#define CDB_H

#include "tomasulo.hpp"
#include "reservationStation.hpp"

class CommonDataBus
{
private:
    bool free;
    TAG tag;
    int32_t value;

public:
    CommonDataBus()
    {
        free = true;
        tag = "";
        value = 0;
    }

    void broadcast(TAG t, int32_t v)
    {
        free = false;
        tag = t;
        value = v;
    }

    void clear()
    {
        free = true;
        tag = "";
        value = 0;
    }

    TAG getTag()
    {
        return tag;
    }

    int32_t getValue()
    {
        return value;
    }
};

#endif