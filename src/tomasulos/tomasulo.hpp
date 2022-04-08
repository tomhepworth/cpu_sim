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
    runnable_program *program;
    CommonDataBus *commonDataBus;
    TomasulosCPU() {}
};

#endif