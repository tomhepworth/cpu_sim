#ifndef RES_STATION_H
#define RES_STATION_H

#include <vector>
#include "isa.h"
#include "reservationStation.hpp"
#include "cdb.hpp"
#include "debug_utils.h"
#include <iostream>

// Forward Declarations
class CommonDataBus;
typedef std::string TAG;

class RegisterStatusEntry
{
public:
    REGISTER_ABI_NAME r;
    TAG tag;
    bool valid;
    int32_t value;

    RegisterStatusEntry(){};
};

// Note: Could be called RegisterRenameTable
class RegisterStatusTable
{
public:
    std::vector<RegisterStatusEntry *> entries;
    CommonDataBus *cdb;

    RegisterStatusTable()
    {
        // Keep a reference to the CDB for updating registers
        // cdb = _cdb;

        // Allocate as many entries as we have registers
        entries = std::vector<RegisterStatusEntry *>();
        entries.reserve(REGABI_UNUSED);

        for (int i = 0; i < (int)REGABI_UNUSED; i++)
        {
            RegisterStatusEntry *entry = new RegisterStatusEntry();
            entry->r = (REGISTER_ABI_NAME)i;
            entry->tag = ""; // no tag means not busy
            entry->valid = true;
            entry->value = 0;

            entries.push_back(entry);
        }

        IF_DEBUG(std::cout << "dont forget to set cdb!" << std::endl);
    }

    TAG getRegTag(REGISTER_ABI_NAME i)
    {
        assert(i < REGABI_UNUSED);
        return entries.at(i)->tag;
    }

    bool getRegValid(REGISTER_ABI_NAME i)
    {
        assert(i < REGABI_UNUSED);
        return entries.at(i)->valid;
    }
    int32_t getRegValue(REGISTER_ABI_NAME i)
    {
        assert(i < REGABI_UNUSED);
        return entries.at(i)->value;
    }
    void setRegTag(REGISTER_ABI_NAME i, TAG t)
    {
        assert(i < REGABI_UNUSED);
        entries.at(i)->tag = t;
    }
    void setRegValid(REGISTER_ABI_NAME i, bool v)
    {
        assert(i < REGABI_UNUSED);
        entries.at(i)->valid = v;
    }
    void setRegValue(REGISTER_ABI_NAME i, int32_t v)
    {
        assert(i < REGABI_UNUSED);
        entries.at(i)->value = v;
    }

    bool setValByTag(TAG t, int32_t v)
    {
        bool tagWasSet = false;
        for (size_t i = 0; i < REGABI_UNUSED; i++)
        {
            if (entries.at(i)->tag == t)
            {
                entries.at(i)->value = v;
                entries.at(i)->valid = true;
                tagWasSet = true;
            }
        }
        return tagWasSet;
    }

    void print()
    {
        std::cout << "PC:" << entries[PC]->value << std::endl;
        std::cout << "REG\tTAG\t\tVALID\tVAL\tREG\tTAG\t\tVALID\tVAL\tREG\tTAG\t\tVALID\tVAL\tREG\tTAG\t\tVALID\tVAL" << std::endl;
        int columnSize = entries.size() / 4;

        for (int i = 0; i < columnSize; i++)
        {
            std::cout << getStringFromRegName(entries[0 * columnSize + i]->r) << "\t" << entries[0 * columnSize + i]->tag << "\t\t" << entries[0 * columnSize + i]->valid << "\t" << entries[0 * columnSize + i]->value;
            std::cout << " |\t" << getStringFromRegName(entries[1 * columnSize + i]->r) << "\t" << entries[1 * columnSize + i]->tag << "\t\t" << entries[1 * columnSize + i]->valid << "\t" << entries[1 * columnSize + i]->value;
            std::cout << " |\t" << getStringFromRegName(entries[2 * columnSize + i]->r) << "\t" << entries[2 * columnSize + i]->tag << "\t\t" << entries[2 * columnSize + i]->valid << "\t" << entries[2 * columnSize + i]->value;
            std::cout << " |\t" << getStringFromRegName(entries[3 * columnSize + i]->r) << "\t" << entries[3 * columnSize + i]->tag << "\t\t" << entries[3 * columnSize + i]->valid << "\t" << entries[3 * columnSize + i]->value << std::endl;
        }
    }
};

#endif