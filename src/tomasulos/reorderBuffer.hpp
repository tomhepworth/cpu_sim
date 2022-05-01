#ifndef REORDERBUFFER_H
#define REORDERBUFFER_H

#include "reservationStation.hpp"

class ReorderBufferEntry
{
public:
    TAG destinationTag;
    int32_t destinationVal;
    int32_t storeAddr;
    int32_t storeData;
    int32_t PCValue;
    bool valid;
    bool ROBemptyFlag; // Returned true by ROB if the ROB was empty
    // maybe... exception, control bits?

    ReorderBufferEntry(TAG _destinationTag, int32_t _destinationVal, int32_t _storeAddr, int32_t _storeData, int32_t _PCValue);

    void print();
};

class ReorderBuffer
{
public:
    // Note, oldest entry should always be at the tail ready to be popped
    //      because the instrucitons are dispatched in-order and so enter the ROB in-order

    ReorderBufferEntry **buffer;
    int max;
    int count;
    int head;
    int tail;
    bool full;

    CommonDataBus *cdb;

    ReorderBuffer(int _size, CommonDataBus *_cdb);

    void push(ReorderBufferEntry *entry);
    ReorderBufferEntry *pop();
    void print();
};

#endif // end header guard