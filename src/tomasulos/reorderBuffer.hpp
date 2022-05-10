#ifndef REORDERBUFFER_H
#define REORDERBUFFER_H

#include "reservationStation.hpp"
#include "cdb.hpp"

class TomasulosCPU; // Forward declaration

class ReorderBufferEntry
{
public:
    OPCODE op; // just for reference really
    REGISTER_ABI_NAME physicalRegisterDestination;
    TAG destinationTag;
    int32_t destinationVal;
    int32_t storeAddr;
    int32_t storeData;
    int32_t PCValue;
    bool valid;
    bool ROBemptyFlag; // Returned true by ROB if the ROB was empty
    // maybe... exception, control bits?

    ReorderBufferEntry(OPCODE _op, TAG _destinationTag, int32_t _destinationVal, int32_t _storeAddr, int32_t _storeData, int32_t _PCValue, REGISTER_ABI_NAME _physicalRegisterDestination);

    void print();
};

// Reorder buffer is basically just a circular buffer with some extra logic
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

    int32_t committedThisCycle; // Just used for stats

    TomasulosCPU *cpu; // Just used to trigger flush
    CommonDataBus *cdb;
    int32_t *memory;
    int32_t *physicalRegisters;

    ReorderBuffer(int _size, CommonDataBus *_cdb, int32_t *_memory, int32_t *physicalRegisters, TomasulosCPU *_cpu);

    // Add an entry to the ROB, return its index to be used as a reference to it
    int32_t push(ReorderBufferEntry *entry);
    ReorderBufferEntry *pop();

    // Find and update a field in the ROB, isStore should be true if we want to update storeAddr and storeData, otherwise we will use register destination and value
    void updateField(OPCODE op, TAG destinationTag, int32_t newVal, int32_t newStoreAddr, int32_t newStoreData, bool valid, bool isStore);
    void triggerFlush();
    void print();
    void Cycle();
};

#endif // end header guard