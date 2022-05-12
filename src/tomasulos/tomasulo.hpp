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
#include "BranchTargetBuffer.hpp"

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
private:
    int32_t numberOfInstructionsCommittedThisCycle = 0;

public:
    // Stats to record
    int32_t correct_predictions;
    int32_t incorrect_predictions;
    int32_t decodes;
    int32_t cycles;                      // How many cycles the processor took to execute the program
    int32_t stalls;                      // How many cycles the processor was stalled during execution
    float mean_ipc;                      // Average (Mean) instructions per cycle
    int32_t total_executed_instructions; // Number of instrucitons executed (committed to cdb) over the entire execution

    runnable_program *program;
    int32_t memorySize;
    int32_t *memory;
    int32_t *physicalRegisters;
    CommonDataBus *cdb;
    ReservationStationTable *reservationStationTable;
    RegisterStatusTable *registerStatusTable;
    BranchTargetBuffer *btb;
    ReorderBuffer *rob;

    // Functional units
    int n_adders, n_loadStores, n_decoders;
    std::vector<TomasulosDecoder *> decoders;
    std::vector<AdderUnit *> adders;
    std::vector<LoadStoreUnit *> loadStoreUnits;

    // Instantiate and set up a tomasulos CPU
    TomasulosCPU() {}
    TomasulosCPU(runnable_program *prog, std::vector<int32_t> *data, int32_t _memorySize, int numberOfDecoders, int numberOfAdders, int numberOfLoadStores);

    // Start cycling
    void Run(int speed, bool step);

    // Perform a cycle of the tomasulos cpu, return true to halt the cpu
    bool Cycle();

    void memDump();

    void flush();
};

#endif
