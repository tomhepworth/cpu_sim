#include "reorderBuffer.hpp"
#include "debug_utils.h"
#include <iostream>

ReorderBufferEntry::ReorderBufferEntry(OPCODE _op, TAG _destinationTag, int32_t _destinationVal, int32_t _storeAddr, int32_t _storeData, int32_t _PCValue)
{
    op = _op;
    destinationTag = _destinationTag;
    destinationVal = _destinationVal;
    storeAddr = _storeAddr;
    storeData = _storeData;
    PCValue = _PCValue;
    valid = false;
};

void ReorderBufferEntry::print()
{
    std::cout << getStringFromOpcode(op) << "\t" << destinationTag << "\t\t" << destinationVal << "\t\t" << storeAddr << "\t\t" << storeData << "\t" << PCValue << "\t" << valid; // Delibarately avoid std::endl to output head tail posiiton ins ROB::print()
}

ReorderBuffer::ReorderBuffer(int _size, CommonDataBus *_cdb, int32_t *_memory)
{
    cdb = _cdb;
    memory = _memory;
    max = _size;
    full = false;
    tail = 0;
    head = 0;
    count = 0;

    buffer = new ReorderBufferEntry *[max];
}

int32_t ReorderBuffer::push(ReorderBufferEntry *entry)
{
    if (count == max) // ROB FULL
    {
        // Handle error
        std::cout << "ROB FULL! - stall" << std::endl;
        return -1;
    }

    // Copy data into buffer, keep track of index
    buffer[head] = entry;
    int32_t entryIndex = head;

    // Move head forward one
    head = (head + 1) % max;
    count++;

    std::cout << "rob returning " << entryIndex << std::endl;
    return entryIndex;
}

ReorderBufferEntry *ReorderBuffer::pop()
{
    ReorderBufferEntry *ret;
    // If ROB is empty return a ROBentry with the empty flag set
    if (count == 0)
    {
        // Empty cant pop
        ret = new ReorderBufferEntry(NOP, "EMPTY", -1, -1, -1, -1);
        ret->ROBemptyFlag = true;
        return ret;
    }

    // Get the value at tail
    ret = buffer[tail];

    // increment tail & reduce count
    tail = (tail + 1) % max;
    count--;

    // std::cout << "TAIL NOW: " << tail << std::endl;
    return ret;
}

void ReorderBuffer::print()
{
    std::cout << "OP\tDEST_TAG\tDEST_VAL\tSTO_ADDR\tSTO_VAL\tPC\tVALID\t HEAD: " << head << "\tTAIL: " << tail << "\tCOUNT: " << count << std::endl;
    for (int i = 0; i < max; i++)
    {
        // std::cout << " i is " << i << std::endl;
        /* _ = empty, X = populated
        Head before tail: [X,_,X,X] Need to be bigger than tail OR less than head
                             ^ ^
                             H T
        Tail before head: [X,X,_,_] Need to be bigger than tail AND less than head
                           ^   ^
                           T   H
        Head == Tail, either empty or full. if count > 0 then print
        */
        if ((tail < head && i >= tail && i < head) ||
            (head < tail && (i >= tail || i < head)) ||
            (head == tail && count > 0))
        {
            buffer[i]->print();
            if (i == tail)
            {
                std::cout << "<-- TAIL";
            }

            if (i == head)
            {
                std::cout << "<-- HEAD";
            }

            std::cout << std::endl;
        }
        else
        {
            // std::cout << "empty" << std::endl;
        }
    }
}

/*
On a cycle the ROB shoud...

- Peak at oldest instruction in the rob
- if it is ready to be committed, pop it and broadcast on CDB
- otherwise do nothing

*/
void ReorderBuffer::Cycle()
{
    // Should never have a negative number of elements
    assert(count >= 0);

    // Do nothing if empty
    if (count == 0)
        return;

    // Look at oldest instruction
    ReorderBufferEntry *oldest = buffer[tail];

    if (oldest->valid)
    {
        std::cout << "ROB COMMITTING TAG : " << oldest->destinationTag << std::endl;

        // If we are committing a halt, end everything
        if (oldest->op == HLT)
        {
            std::cout << "HALT EXCEPTION COMMITTING! " << std::endl;
            exit(0);
        }

        // Commit on cdb, or for stores, write to memory
        switch (oldest->op)
        {
        // case SB:
        case SW:
            // Commit a store to memory
            memory[oldest->storeAddr] = oldest->storeData;
            break;
        default:
            // Anything other than stores just needs to be broadcast on CDB
            break;
        }

        cdb->broadcast(oldest->destinationTag, oldest->destinationVal);

        /*
            Note on memory disambiguation...
            As dependencies on stores are possible it would seem we should broadcast a store on the CDB.
            However, we wait for all previous stores to be completed before a load is done, so these dependencies cant cause problems
            and the RAW memory dependencies shouldnt show up in the RST
        */

        pop(); // Pop instruction out of the ROB, dont bother using the result
    }
}

void ReorderBuffer::updateField(OPCODE op, TAG destinationTag, int32_t newVal, int32_t newStoreAddr, int32_t newStoreData, bool valid, bool isStore)
{
    std::cout << "ROB UPDATING TAG : " << destinationTag << std::endl;
    bool success = false;
    for (int i = 0; i < max; i++)
    {
        // Logic to exclude empty elements in circular buffer
        if ((tail < head && i >= tail && i < head) ||
            (head < tail && (i >= tail || i < head)) ||
            (head == tail && count > 0))
        {
            // If the element is populated then check if it is the right one to update
            if (destinationTag == buffer[i]->destinationTag)
            {
                // Update
                if (isStore)
                {
                    // Update memory destination and value
                    buffer[i]->storeAddr = newStoreAddr;
                    buffer[i]->storeData = newStoreData;
                }
                else
                {
                    // Update register destination and value
                    buffer[i]->destinationVal = newVal;
                }

                // If appropriate, mark as valid so entry can be committed from ROB
                buffer[i]->valid = valid;
                success = true;
            }
        }
    }

    // We should never NOT find an instruction in the ROB
    assert(success);
}