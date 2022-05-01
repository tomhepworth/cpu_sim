#include "reorderBuffer.hpp"
#include <iostream>

ReorderBufferEntry::ReorderBufferEntry(TAG _destinationTag, int32_t _destinationVal, int32_t _storeAddr, int32_t _storeData, int32_t _PCValue)
{
    destinationTag = _destinationTag;
    destinationVal = _destinationVal;
    storeAddr = _storeAddr;
    storeData = _storeData;
    PCValue = _PCValue;
};

void ReorderBufferEntry::print()
{
    std::cout << destinationTag << "\t" << destinationVal << "\t" << storeAddr << "\t" << storeData << "\t" << PCValue << "\t" << valid; // Delibarately avoid std::endl to output head tail posiiton ins ROB::print()
}

ReorderBuffer::ReorderBuffer(int _size, CommonDataBus *_cdb)
{
    cdb = _cdb;

    max = _size;
    full = false;
    tail = 0;
    head = 0;
    count = 0;

    buffer = new ReorderBufferEntry *[max];
}

void ReorderBuffer::push(ReorderBufferEntry *entry)
{
    if (count == max)
    {
        // Handle error
        std::cout << "ROB FULL! - stall" << std::endl;
    }

    // Copy data into buffer
    buffer[head] = entry;

    // Move head forward one
    head = (head + 1) % max;
    count++;
}

ReorderBufferEntry *ReorderBuffer::pop()
{
    ReorderBufferEntry *ret;
    // If ROB is empty return a ROBentry with the empty flag set
    if (count == 0)
    {
        // Empty cant pop
        ret = new ReorderBufferEntry("EMPTY", -1, -1, -1, -1);
        ret->ROBemptyFlag = true;
        return ret;
    }

    // Get the value at tail
    ret = buffer[tail];

    // increment tail & reduce count
    tail = (tail + 1) % max;
    count--;
}

void ReorderBuffer::print()
{
    std::cout << "DEST_TAG\tDEST_VAL\tSTO_ADDR\tSTO_VAL\tPC\tVALID\t HEAD: " << head << "\tTAIL: " << tail << "\tCOUNT: " << count << std::endl;
    int i = tail;
    while (i < ((tail + count - 1) % max))
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

        i = (i + 1) % max;
    }
}