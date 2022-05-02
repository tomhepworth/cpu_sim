#ifndef TOMASULO_DECODER_H
#define TOMASULO_DECODER_H

#include "isa.h"
#include "instruction.h"
#include "cpu.h"
#include "regStatus.hpp"
#include "reorderBuffer.hpp"
#include "debug_utils.h"
/*
In my simulator instructions are never binary so dont really need to be decoded

The decode unit in my tomasulos implementation just serves to pass the instruction
to the correct reservation station, doing any renaming, and stall if there is none available
*/

class TomasulosDecoder
{
public:
  runnable_program *program;
  RegisterStatusTable *registerStatusTable;
  ReservationStationTable *reservationStationTable;
  ReorderBuffer *rob;

  Instruction *currentInstruction;

  OPCODE opcode;
  REGISTER_ABI_NAME rs1;
  REGISTER_ABI_NAME rs2;
  REGISTER_ABI_NAME rd;
  int32_t imm;
  bool stalled;

  TAG lastUpdatedRS;

  TomasulosDecoder(){};

  TomasulosDecoder(runnable_program *p, RegisterStatusTable *statusTable, ReservationStationTable *stationTable, ReorderBuffer *_rob)
  {
    stalled = false;
    program = p;
    registerStatusTable = statusTable;
    reservationStationTable = stationTable;
    rob = _rob;
  }

  /*Take instruction from instruction memory, "decode", pass to available reservation station, stall if unable to pass
    The decode stage should rename registers where appropriate, or pass values straight in
    Return true if stalled */
  bool Cycle();

  void print();
};

#endif