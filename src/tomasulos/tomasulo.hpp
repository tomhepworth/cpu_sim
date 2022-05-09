#ifndef TOMASULO_H
#define TOMASULO_H

#include <string>
#include "isa.h"
#include "cpu.h"
#include "cdb.hpp"
#include "reservationStation.hpp"
#include "regStatus.hpp"
#include "decoder.hpp"
#include "functionalUnit.hpp"
#include "reorderBuffer.hpp"

/* Points by Onar Mutlu:
1: If reservation station available before renaming
    - Instructuren + renamed operands (source value/tag) inserted into reservation station
    - Only rename if reservation station is available
   Else stall

2: While in reservation station, each instruction watches the CDB for tag of its sources
 - Listen for tag of sources
 - When tag seen, grab value for source and keep it in the reservation station
 - When both operands available, instruciton is ready to be dispatched

3: Dispatch instruction to functional unit when instruction is ready

4: After instruction is finished in the functional unit
    - Arbitrate for CDB
    - Put tagged value onto CDB (broadcast)
    - Register file is connected to the CDB
        - Register file contains a tag indicating the latest writer to the register
        - if the tag in the register file matches the broadcast tag,
            write broadcast value into register (and set valid bit)
    - Reclaim rename tag
        - dont have copies of tags in the system
*/

class TomasulosCPU
{
public:
    // Stats to record
    int32_t cycles;   // How many cycles the processor took to execute the program
    int32_t stalls;   // How many cycles the processor was stalled during execution
    int32_t mean_ipc; // Average (Mean) instructions per cycle

    runnable_program *program;
    int32_t memorySize;
    int32_t *memory;
    CommonDataBus *cdb;
    ReservationStationTable *reservationStationTable;
    RegisterStatusTable *registerStatusTable;
    ReorderBuffer *rob;

    // Functional units
    TomasulosDecoder *decoder;
    AdderUnit *adder;
    LoadStoreUnit *loadStoreUnit;

    // Instantiate and set up a tomasulos CPU
    TomasulosCPU() {}
    TomasulosCPU(runnable_program *prog, int32_t _memorySize);

    // Start cycling
    void Run(int speed, bool step);

    // Perform a cycle of the tomasulos cpu, return true to halt the cpu
    bool Cycle();

    void memDump();
};

#endif
