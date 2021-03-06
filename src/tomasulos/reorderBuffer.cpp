#include "reorderBuffer.hpp"
#include "tomasulo.hpp"
#include "debug_utils.h"
#include <iostream>

ReorderBufferEntry::ReorderBufferEntry(OPCODE _op, TAG _destinationTag, int32_t _destinationVal, int32_t _storeAddr, int32_t _storeData, int32_t _PCValue, REGISTER_ABI_NAME _physicalRegisterDestination)
{
    op = _op;
    destinationTag = _destinationTag;
    destinationVal = _destinationVal;
    storeAddr = _storeAddr;
    storeData = _storeData;
    PCValue = _PCValue;
    valid = false;
    physicalRegisterDestination = _physicalRegisterDestination;
};

void ReorderBufferEntry::print()
{
    std::cout << getStringFromOpcode(op) << "\t" << destinationTag << "\t\t" << destinationVal << "\t\t" << storeAddr << "\t\t" << storeData << "\t" << PCValue << "\t" << valid << "\t" << getStringFromRegName(physicalRegisterDestination); // Delibarately avoid std::endl to output head tail posiiton ins ROB::print()
}

ReorderBuffer::ReorderBuffer(int _size, CommonDataBus *_cdb, int32_t *_memory, int32_t *_physicalRegisters, TomasulosCPU *_cpu, BranchTargetBuffer *_btb)
{
    cpu = _cpu;
    cdb = _cdb;
    memory = _memory;
    physicalRegisters = _physicalRegisters;
    max = _size - 1;
    full = false;
    tail = 0;
    head = 0;
    count = 0;
    btb = _btb;

    buffer = new ReorderBufferEntry *[max];
}

int32_t ReorderBuffer::push(ReorderBufferEntry *entry)
{
    if (count == max) // ROB FULL
    {
        // Handle error
        // std::cout << "ROB FULL! - stall! tag:" << entry->destinationTag << std::endl;
        return -1;
    }

    // Copy data into buffer, keep track of index
    buffer[head] = entry;
    int32_t entryIndex = head;

    // Move head forward one
    head = (head + 1) % max;
    count++;

    return entryIndex;
}

ReorderBufferEntry *ReorderBuffer::pop()
{
    ReorderBufferEntry *ret;
    // If ROB is empty return a ROBentry with the empty flag set
    if (count == 0)
    {
        // Empty cant pop
        ret = new ReorderBufferEntry(NOP, "EMPTY", -1, -1, -1, -1, REGABI_UNUSED);
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
    std::cout << "OP\tDEST_TAG\tDEST_VAL\tSTO_ADDR\tSTO_VAL\tPC\tVALID\tPHYS\tHEAD: " << head << "\tTAIL: " << tail << "\tCOUNT: " << count << std::endl;
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

void ReorderBuffer::triggerFlush()
{
    cpu->flush();

    while (count > 0)
        pop();

    return;
}
/*
On a cycle the ROB shoud...

- Peak at oldest instruction in the rob
- if it is ready to be committed, pop it and broadcast on CDB
- otherwise do nothing

*/
void ReorderBuffer::Cycle()
{
    committedThisCycle = 0;
    // Should never have a negative number of elements
    assert(count >= 0);

    // Do nothing if empty
    if (count == 0)
        return;

    // Look at oldest instruction
    ReorderBufferEntry *oldest = buffer[tail];

    // Commit ready instructions in order until there are no more
    while (oldest->valid)
    {

        // IF_DEBUG(std::cout << "ROB COMMITTING TAG : " << oldest->destinationTag << std::endl);

        // If we are committing a halt, end everything
        if (oldest->op == HLT)
        {
            // IF_DEBUG(std::cout << "HALT EXCEPTION COMMITTING! " << std::endl);
            exit(0);
        }

        // BRANCH HANDLING
        bool flushed = false;
        if (oldest->physicalRegisterDestination == PC) // RD is PC only for branch instructions
        {
            if (btb->mode == 0) // two bit
            {
                // The target PC is the branch we took.
                int32_t targetPC = cpu->btb->targets[oldest->PCValue & btb->size];
                if (oldest->destinationVal != targetPC)
                {
                    // Predicted incorrectly so flush
                    // IF_DEBUG(std::cout << "ROB: PREDICTED INCORRECTLY" << std::endl);

                    triggerFlush();
                    flushed = true;

                    // Also update btb
                    btb->targets[oldest->PCValue & btb->size] = oldest->destinationVal;

                    // if the branch was not taken then destVal will be pc+1
                    if (oldest->destinationVal != oldest->PCValue + 1)
                    {
                        // Branch not taken so subtract from prediction bits if > 0
                        if (btb->predictionBits[oldest->PCValue & cpu->btb->size] < 3)
                            btb->predictionBits[oldest->PCValue & cpu->btb->size]++;
                    }
                    else
                    {
                        // Branch taken so add to prediction bits if < 3
                        if (btb->predictionBits[oldest->PCValue & cpu->btb->size] > 0)
                            btb->predictionBits[oldest->PCValue & cpu->btb->size]--;
                    }

                    cpu->incorrect_predictions++;
                }
                else
                {
                    // Predicted correctly so just carry on
                    // IF_DEBUG(std::cout << "ROB: PREDICTED CORRECTLY" << std::endl);
                    cpu->correct_predictions++;
                }
            }
            else if (btb->mode == 1) // always take
            {
                if (oldest->destinationVal == oldest->PCValue + 1)
                {
                    // IF_DEBUG(std::cout << "FLUSHING" << std::endl);
                    triggerFlush();
                    flushed = true;
                    cpu->incorrect_predictions++;
                }
                else
                {
                    cpu->correct_predictions++;
                }
            }
            else if (btb->mode == 2) // never take
            {
                if (oldest->destinationVal != oldest->PCValue + 1)
                {
                    // IF_DEBUG(std::cout << "FLUSHING" << std::endl);
                    triggerFlush();
                    flushed = true;
                    cpu->incorrect_predictions++;
                }
                else
                {
                    cpu->correct_predictions++;
                }
            }
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
            /*
                Update physical register file, except if we have flushed and rd == pc
                We will only have flushed if rd = PC. If we didnt flush then the speculative execution was correct, so no need to change the PC.
            */
            if (!(!flushed && oldest->physicalRegisterDestination == PC))
                physicalRegisters[oldest->physicalRegisterDestination] = oldest->destinationVal;

            break;
        }

        // IF_DEBUG(std::cout << "COMITTED: " << getStringFromOpcode(oldest->op) << std::endl);
        cdb->broadcast(oldest->destinationTag, oldest->destinationVal);

        /*
            Note on memory disambiguation...
            As dependencies on stores are possible it would seem we should broadcast a store on the CDB.
            However, we wait for all previous stores to be completed before a load is done, so these dependencies cant cause problems
            and the RAW memory dependencies shouldnt show up in the RST
        */

        pop();                // Pop instruction out of the ROB, dont bother using the result
        committedThisCycle++; // Increment this every time we commit something

        oldest = buffer[tail];

        // Do nothing if empty
        if (count == 0)
            return;
    }
}

void ReorderBuffer::updateField(OPCODE op, TAG destinationTag, int32_t newVal, int32_t newStoreAddr, int32_t newStoreData, bool valid, bool isStore)
{
    // IF_DEBUG(std::cout << "ROB UPDATING TAG : " << destinationTag << "OP: " << getStringFromOpcode(op) << std::endl);

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