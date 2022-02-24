#include "cpu.h"

#ifndef TEST_MODE
#include "debug_utils.h"
#endif

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <chrono>
#include <thread>

Pipeline::Pipeline()
{
    // Initialise stages to 0, -1, -2 ... -(STAGE_COUNT - 1)
    for (int i = 0; i < STAGE_COUNT; i++)
    {
        stageIndexes[i] = -1;
    }
}

bool Pipeline::Advance(CPU *cpu, runnable_program *program, Scoreboard *scoreboard)
{

    // // Shift all elements one place right and set FETCH to PC value
    bool piplineFrozen = false;
    for (int i = STAGE_COUNT; i >= 1; i--)
    {
        if (stageIndexes[i - 1] >= 0)
        {
            // If instruction has finished its stage, shift it to the next stage in pipeline
            if (program->at(stageIndexes[i - 1])->free[i - 1])
            {
                if (i == 4 && stageIndexes[i] > 0)
                {
                    // IF inst is leaving pipeline, reset it
                    // printf("Instruction %d leaving pipeline\n", i);
                    program->at(stageIndexes[i])->reset();
                }

                // printf("Swapping %d to %d's position\n", i, i - 1);
                stageIndexes[i] = stageIndexes[i - 1];
                continue;
            }
            else // If it hadn't finished the stage, freeze the pipeline behind it
            {

                // printf("Freezing pipe, setting position %d to empty\n", i);
                stageIndexes[i] = -1; // -1 indicates no instruction is in this stage
                piplineFrozen = true;
                break;
            }
        }
        else
        {
            if (i == 4 && stageIndexes[i] > 0)
            {
                // IF inst is leaving pipeline, reset it
                // printf("Instruction %d leaving pipeline\n", program->at(stageIndexes[i])->linenum);
                program->at(stageIndexes[i])->reset();
            }

            // printf("Swapping empty position\n");
            stageIndexes[i] = stageIndexes[i - 1];
        }
    }

    // // Shift along pipeline
    // for (int stage_index = WRITEBACK; stage_index != FETCH; stage_index--)
    // {
    //     CPU_STAGE stage = static_cast<CPU_STAGE>(stage_index);
    //     bool swap = false;

    //     // Just to stop segfault of doing program.at(-1)
    //     if (!(stageIndexes[stage] < 0))
    //     {
    //         // If this unit has finished with its current instruction
    //         if (program->at(stageIndexes[stage])->free[stage])
    //         {
    //             // Reset and move it out of pipeline
    //             if (stage == WRITEBACK)
    //                 program->at(stageIndexes[WRITEBACK])->reset();

    //             // IF previous stage had finished too then move it into this slot
    //             if (program->at(stageIndexes[stage - 1])->free[stage - 1])
    //             {
    //                 swap = true;
    //             }
    //             else // Otherwise default to -1 to show the unit is empty
    //             {
    //                 stageIndexes[stage] = -1;
    //             }
    //         }
    //     }

    //     if (swap)
    //         stageIndexes[stage] = stageIndexes[stage - 1];
    // }

    // if (!(stageIndexes[FETCH] < 0) && !(stageIndexes[DECODE] < 0))
    // {
    //     if (!(program->at(stageIndexes[FETCH])->free[FETCH] && program->at(stageIndexes[DECODE])->free[DECODE]))
    //         piplineFrozen = true;
    // }

    // If pipeline wasnt frozen, update instruction to be fetched
    if (!piplineFrozen)
    {
        if (scoreboard->isValid(PC))
        {
            stageIndexes[FETCH] = cpu->registers[PC];
        }
        else
        {
            stageIndexes[FETCH] = -1;
        }
    }

#ifndef TEST_MODE
    if (debug)
    {
        std::cout << "PC:\t" << cpu->registers[PC] << std::endl;
        std::cout << "F:\t" << std::to_string(stageIndexes[FETCH]) << "\t" << ((stageIndexes[FETCH] < 0) ? "unit empty" : program->at(stageIndexes[FETCH])->rawText + "\t\t" + std::to_string(program->at(stageIndexes[FETCH])->linenum)) << std::endl;
        std::cout << "D:\t" << std::to_string(stageIndexes[DECODE]) << "\t" << ((stageIndexes[DECODE] < 0) ? "unit empty" : program->at(stageIndexes[DECODE])->rawText + "\t\t" + std::to_string(program->at(stageIndexes[DECODE])->linenum)) << std::endl;
        std::cout << "E:\t" << std::to_string(stageIndexes[EXECUTE]) << "\t" << ((stageIndexes[EXECUTE] < 0) ? "unit empty" : program->at(stageIndexes[EXECUTE])->rawText + "\t\t" + std::to_string(program->at(stageIndexes[EXECUTE])->linenum)) << std::endl;
        std::cout << "M:\t" << std::to_string(stageIndexes[MEMORY]) << "\t" << ((stageIndexes[MEMORY] < 0) ? "unit empty" : program->at(stageIndexes[MEMORY])->rawText + "\t\t" + std::to_string(program->at(stageIndexes[MEMORY])->linenum)) << std::endl;
        std::cout << "W:\t" << std::to_string(stageIndexes[WRITEBACK]) << "\t" << ((stageIndexes[WRITEBACK] < 0) ? "unit empty" : program->at(stageIndexes[WRITEBACK])->rawText + "\t\t" + std::to_string(program->at(stageIndexes[WRITEBACK])->linenum)) << std::endl;
    }
#endif

    /*
        Probably could make this more efficient
    */
    bool didFetch = true,
         didDecode = true, didExec = true, didMem = true, didWB = true;
    if (stageIndexes[WRITEBACK] >= 0)
        didWB = program->at(stageIndexes[WRITEBACK])->writeBack(cpu, scoreboard);

    if (stageIndexes[MEMORY] >= 0)
        didMem = program->at(stageIndexes[MEMORY])->memoryAccess(cpu, scoreboard);

    if (stageIndexes[EXECUTE] >= 0)
        didExec = program->at(stageIndexes[EXECUTE])->execute(cpu, scoreboard);

    if (stageIndexes[DECODE] >= 0)
        didDecode = program->at(stageIndexes[DECODE])->decode(cpu, scoreboard);

    if (stageIndexes[FETCH] >= 0)
        didFetch = program->at(stageIndexes[FETCH])->fetch(cpu, scoreboard);

#ifndef TEST_MODE
    if (debug)
    {
        if (!didFetch)
            std::cout << "Couldnt Fetch " << std::to_string(program->at(stageIndexes[FETCH])->linenum) << std::endl;
        if (!didDecode)
            std::cout << "Couldnt Decode " << std::to_string(program->at(stageIndexes[DECODE])->linenum) << std::endl;
        if (!didExec)
            std::cout << "Couldnt Execute " << std::to_string(program->at(stageIndexes[EXECUTE])->linenum) << std::endl;
        if (!didMem)
            std::cout << "Couldnt Memory Access " << std::to_string(program->at(stageIndexes[MEMORY])->linenum) << std::endl;
        if (!didWB)
            std::cout << "Couldnt Write Back " << std::to_string(program->at(stageIndexes[WRITEBACK])->linenum) << std::endl;
    }
#endif

    return !piplineFrozen;
}

CPU::CPU()
{
    cycles = 0;
    pipeline = new Pipeline();
    scoreboard = new Scoreboard();
    memorySize = 512;
    memory = (int32_t *)calloc(memorySize, sizeof(int32_t));
    speed = 10;
    running = true;
}

CPU::CPU(Pipeline *_pipeline, Scoreboard *_scoreboard, int _memorySize, int _speed)
{
    cycles = 0;
    pipeline = _pipeline;
    scoreboard = _scoreboard;
    memorySize = _memorySize;
    memory = (int32_t *)calloc(memorySize, sizeof(int32_t));
    registers = (int32_t *)calloc(REG_UNUSED, sizeof(int32_t));
    speed = _speed;
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

#ifndef TEST_MODE
    if (debug)
        std::cout << "----------- Cylcle #" << cycles << "-----------" << std::endl;
#endif

    assert(running);

    // If the program counter value is greater than the size of the program, just send NOP forever
    if (!(registers[PC] < (int32_t)program->size()))
    {
        // printf("PC beyond end of program on cycle %d\n", cycles); // debug
        program->push_back(new Instruction()); // add NOP to end of program
    }

    // Advance the pipeline
    bool pipelineAdvanced = pipeline->Advance(this, program, scoreboard);

    // If the pipeline advanced we can move the pc along
    if (pipelineAdvanced)
    {
        // Increment PC
        registers[PC]++;
    }
    // else
    // {
    //     std::cout << "not incing pc" << std::endl;
    // }
    // Increment cycle count
    cycles++;

    std::this_thread::sleep_for(std::chrono::milliseconds(speed));

    // Fetch - CPU reads instructions from the address in the memory whose value is present in the program counter
    // Decode - instruction is decoded and the register file is accessed to get the values from the registers used in the instruction.
    // Execute - ALU operations are performed.
    // Memory Access - memory operands are read and written from/to the memory that is present in the instruction.
    // Write Back - computed / fetched value is written back to the register present in the instructions.
}

void CPU::regDump()
{
    std::cout << "------------REG-DUMP-------------" << std::endl;
    for (size_t i = 0; i < REGABI_UNUSED; i++)
    {
        std::cout << i << ":\t" << registers[i] << std::endl;
    }
    std::cout << "---------------------------------" << std::endl;
}

void CPU::memDump(int start, int end)
{
    std::cout << "------------MEM-DUMP-------------" << std::endl;
    for (size_t i = start; i <= end; i++)
    {
        std::cout << "0x" << i << ":\t" << memory[i] << std::endl;
    }
    std::cout << "---------------------------------" << std::endl;
}

int CPU::getMemorySize()
{
    return memorySize;
}