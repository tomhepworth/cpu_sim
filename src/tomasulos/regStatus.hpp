#ifndef RES_STATION_H
#define RES_STATION_H

#include "isa.h"
#include "tomasulo.hpp"
#include "cdb.hpp"

typedef struct
{
    REGISTER_ABI_NAME r;
    TAG tag;
    bool valid;
    int32_t value;

} RegisterStatusEntry;

// Note: Could be called RegisterRenameTable
class RegisterStatusTable
{
public:
    RegisterStatusEntry *entries;
    CommonDataBus *cdb;

    RegisterStatusTable(CommonDataBus *_cdb)
    {
        // Keep a reference to the CDB for updating registers
        cdb = _cdb;

        // Allocate as many entries as we have registers
        entries = new RegisterStatusEntry[REGABI_UNUSED];

        for (int i = 0; i < (int)REGABI_UNUSED; i++)
        {
            entries[i].r = (REGISTER_ABI_NAME)i;
            entries[i].tag = ""; // no tag means not busy
            entries[i].valid = true;
            entries[i].value = 0;
        }
    }

    TAG getRegTag(REGISTER_ABI_NAME i)
    {
        assert(i < REGABI_UNUSED);
        return entries[i].tag;
    }
    bool getRegValid(REGISTER_ABI_NAME i)
    {
        assert(i < REGABI_UNUSED);
        return entries[i].valid;
    }
    int32_t getRegValue(REGISTER_ABI_NAME i)
    {
        assert(i < REGABI_UNUSED);
        return entries[i].value;
    }
    void setRegTag(REGISTER_ABI_NAME i, TAG t)
    {
        assert(i < REGABI_UNUSED);
        entries[i].tag = t;
    }
    void setRegValid(REGISTER_ABI_NAME i, bool v)
    {
        assert(i < REGABI_UNUSED);
        entries[i].valid = v;
    }
    void setRegValue(REGISTER_ABI_NAME i, int32_t v)
    {
        assert(i < REGABI_UNUSED);
        entries[i].value = v;
    }

    bool setValByTag(TAG t, int32_t v)
    {
        bool tagWasSet = false;
        for (size_t i = 0; i < REGABI_UNUSED; i++)
        {
            if (entries[i].tag == t)
            {
                entries[i].value = v;
                entries[i].valid = true;
                tagWasSet = true;
            }
        }
        return tagWasSet;
    }
};

#endif