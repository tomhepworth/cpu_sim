#include "cpu.h"

#include <iostream>
#include <stdlib.h>

Pipeline::Pipeline()
{
    // Initialise stages to 0, -1, -2 ... -(STAGE_COUNT - 1)
    for (int i = 0; i < STAGE_COUNT; i++)
    {
        stageIndexes[i] = -1;
    }
}

bool Pipeline::Advance(runnable_program *program, Scoreboard *scoreboard)
{

    // Shift all elements one place right and set FETCH to PC value
    bool piplineFrozen = false;
    for (int i = STAGE_COUNT; i >= 1; i--)
    {
        if (stageIndexes[i - 1] >= 0)
        {
            // If instruction has finished its stage, shift it to the next stage in pipeline
            if (program->at(stageIndexes[i - 1])->free[i - 1])
            {
                // stageIndexes[i - 1] = -1;
                stageIndexes[i] = stageIndexes[i - 1];
                continue;
            }
            else // If it hadn't finished the stage, freeze the pipeline behind it
            {
                stageIndexes[i] = -1; // -1 indicates no instruction is in this stage
                piplineFrozen = true;
                break;
            }
        }
        else
        {
            stageIndexes[i] = stageIndexes[i - 1];
        }
    }

    // If pipeline wasnt frozen, update instruction to be fetched
    if (!piplineFrozen)
        stageIndexes[FETCH] = registers[PC];

    // Debug
    if (printStages || debug)
    {
        std::cout
            << "---------------------------------------------------------" << std::endl;
        std::cout << "\t\t\tF\tD\tE\tM\tW" << std::endl;
        std::cout << "INSTR ADDR:\t\t" << stageIndexes[FETCH] << "\t" << stageIndexes[DECODE] << "\t" << stageIndexes[EXECUTE] << "\t" << stageIndexes[MEMORY] << "\t" << stageIndexes[WRITEBACK] << std::endl;
        std::cout << "PC IS:" << registers[PC] << " size: " << program->size() << std::endl;
        std::cout << "OPCODE EXECUTING:\t"
                  << ((stageIndexes[FETCH] >= 0) ? program->at(stageIndexes[FETCH])->opcode : 0)
                  << "\t" << ((stageIndexes[DECODE] >= 0) ? program->at(stageIndexes[DECODE])->opcode : 0)
                  << "\t" << ((stageIndexes[EXECUTE] >= 0) ? program->at(stageIndexes[EXECUTE])->opcode : 0)
                  << "\t" << ((stageIndexes[MEMORY] >= 0) ? program->at(stageIndexes[MEMORY])->opcode : 0)
                  << "\t" << ((stageIndexes[WRITEBACK] >= 0) ? program->at(stageIndexes[WRITEBACK])->opcode : 0) << std::endl;
    }

    /*
        Probably could make this more efficient
    */
    bool didFetch = true, didDecode = true, didExec = true, didMem = true, didWB = true;

    if (stageIndexes[FETCH] >= 0)
        didFetch = program->at(stageIndexes[FETCH])->fetch(scoreboard);

    if (stageIndexes[DECODE] >= 0)
        didDecode = program->at(stageIndexes[DECODE])->decode(scoreboard);

    if (stageIndexes[EXECUTE] >= 0)
        didExec = program->at(stageIndexes[EXECUTE])->execute(scoreboard);

    if (stageIndexes[MEMORY] >= 0)
        didMem = program->at(stageIndexes[MEMORY])->memoryAccess(scoreboard);

    if (stageIndexes[WRITEBACK] >= 0)
        didWB = program->at(stageIndexes[WRITEBACK])->writeBack(scoreboard);

    if (debug)
    {
        if (!didFetch)
            std::cout << "Couldnt Fetch" << std::endl;
        if (!didDecode)
            std::cout << "Couldnt Decode" << std::endl;
        if (!didExec)
            std::cout << "Couldnt Execute" << std::endl;
        if (!didMem)
            std::cout << "Couldnt Memory Access" << std::endl;
        if (!didWB)
            std::cout << "Couldnt Write Back" << std::endl;

        // scoreboard->log();
    }

    return !piplineFrozen;
}

CPU::CPU()
{
    cycles = 0;
    pipeline = new Pipeline();
    scoreboard = new Scoreboard();
}

CPU::CPU(Pipeline *_pipeline, Scoreboard *_scoreboard)
{
    cycles = 0;
    pipeline = _pipeline;
    scoreboard = _scoreboard;
}

void CPU::LoadProgram(runnable_program *prog)
{
    program = prog;

    if (program->empty())
    {
        std::cout << "Empty program! Exiting" << std::endl;
        exit(EXIT_FAILURE);
    }
    else
    {
        std::cout << "Loaded program!" << std::endl;
    }
}

void CPU::Cycle()
{
    assert(running);

    // If the program counter value is greater than the size of the program, just send NOP forever
    if (!(registers[PC] < (int32_t)program->size()))
    {
        // printf("PC beyond end of program on cycle %d\n", cycles); // debug
        program->push_back(new Instruction()); // add NOP to end of program
    }

    // Advance the pipeline
    bool pipelineAdvanced = pipeline->Advance(program, scoreboard);

    // If the pipeline advanced we can move the pc along
    if (pipelineAdvanced)
        // Increment PC
        registers[PC]++;

    // Increment cycle count
    cycles++;

    // Fetch - CPU reads instructions from the address in the memory whose value is present in the program counter
    // Decode - instruction is decoded and the register file is accessed to get the values from the registers used in the instruction.
    // Execute - ALU operations are performed.
    // Memory Access - memory operands are read and written from/to the memory that is present in the instruction.
    // Write Back - computed / fetched value is written back to the register present in the instructions.
}
