#include "cpu.h"
#include "debug_utils.h"

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <chrono>
#include <thread>

Pipeline::Pipeline()
{
}

bool Pipeline::Advance(CPU *cpu, runnable_program *program, Scoreboard *scoreboard)
{
    return true;
}

void ScalarPipeline::setCPU(CPU *_cpu)
{
    cpu = _cpu;
    fetchUnit = FetchUnit(cpu);
    decodeUnit = DecodeUnit(cpu);
    executeUnit = ExecuteUnit(cpu);
    memoryAccessUnit = MemoryAccessUnit(cpu);
    writebackUnit = WriteBackUnit(cpu);
}

ScalarPipeline::ScalarPipeline(CPU *_cpu)
{
    cpu = _cpu;
    fetchUnit = FetchUnit(cpu);
    decodeUnit = DecodeUnit(cpu);
    executeUnit = ExecuteUnit(cpu);
    memoryAccessUnit = MemoryAccessUnit(cpu);
    writebackUnit = WriteBackUnit(cpu);
}

bool ScalarPipeline::Advance(CPU *cpu, runnable_program *program, Scoreboard *scoreboard)
{
    bool pipelineAdvanced = false;

    bool completedFetch, completedDecode, completedExecute, completedMemoryAccess, completedWriteback;

    // All units perform their cycle task:
    completedWriteback = writebackUnit.onCycle();
    completedMemoryAccess = memoryAccessUnit.onCycle();
    completedExecute = executeUnit.onCycle();
    completedDecode = decodeUnit.onCycle();
    completedFetch = fetchUnit.onCycle();

    // If the unit completed its task and there is space in the next unit, move data forward

    // Handle exit from pipeline
    if (completedWriteback)
    {
        writebackUnit.setAsEmpty();
    }

    // Handle move to write back unit
    if (completedMemoryAccess && writebackUnit.empty) // If prev stage complete and this stage is ready, update the unit
    {
        if (memoryAccessUnit.empty) // if the previous unit was empty, this unit becomes empty
        {
            writebackUnit.setAsEmpty();
        }
        else // Prev unit not empty, take update this unit with the relevant data
        {
            writebackUnit.AcceptData(memoryAccessUnit.PCValue, memoryAccessUnit.opcode, memoryAccessUnit.rd, memoryAccessUnit.value, memoryAccessUnit.requiresWriteBack);
            memoryAccessUnit.setAsEmpty(); // Mark the previous unit as empty so it can be updated
        }
    }

    // handle move to MemoryAccess unit
    if (completedExecute && memoryAccessUnit.empty)
    {
        if (executeUnit.empty)
        {
            memoryAccessUnit.setAsEmpty();
        }
        else
        {
            memoryAccessUnit.AcceptData(executeUnit.PCValue, executeUnit.opcode, executeUnit.rd, executeUnit.result, executeUnit.requiresWriteBack, executeUnit.requiresMemoryAccess);
            executeUnit.setAsEmpty();
        }
    }

    // handle move to execute unit
    if (completedDecode && executeUnit.empty)
    {
        if (decodeUnit.empty)
        {
            executeUnit.setAsEmpty();
        }
        else
        {
            std::cout << " PASSING RD " << getStringFromRegName(decodeUnit.rd) << " TO EXU" << std::endl;
            executeUnit.AcceptData(decodeUnit.PCValue, decodeUnit.opcode, decodeUnit.rd, decodeUnit.rs1Value, decodeUnit.rs2Value, decodeUnit.imm, decodeUnit.requiresWriteBack, decodeUnit.requiresMemoryAccess);
            decodeUnit.setAsEmpty();
        }
    }

    // Handle move to decode unit
    if (completedFetch && decodeUnit.empty)
    {
        if (fetchUnit.empty)
        {
            decodeUnit.setAsEmpty();
        }
        else
        {
            decodeUnit.AcceptData(fetchUnit.PCValue, fetchUnit.instruction);
            fetchUnit.setAsEmpty();
        }
    }

    if (fetchUnit.empty && (cpu->registers[PC] < (int32_t)program->size()))
    {
        if (debug)
            std::cout << "Fetching new" << std::endl;

        fetchUnit.AcceptData(cpu->registers[PC]);
        pipelineAdvanced = true; // If we could fetch an instruction then we need to update pc, so signal to the cpu to do so
    }

    if (debug)
    {
        std::cout << "F :: "
                  << "empty: " << fetchUnit.empty << "\t" << fetchUnit.instruction->opcode << "\t" << fetchUnit.PCValue << "\tcompleted phase? " << completedFetch << "\t" << (!fetchUnit.empty ? cpu->program->at(fetchUnit.PCValue)->rawText : "none") << std::endl;
        std::cout << "D :: "
                  << "empty: " << decodeUnit.empty << "\t" << decodeUnit.opcode << "\t" << decodeUnit.PCValue << "\tcompleted phase? " << completedDecode << "\t" << (!decodeUnit.empty ? cpu->program->at(decodeUnit.PCValue)->rawText : "none") << std::endl;
        std::cout << "E :: "
                  << "empty: " << executeUnit.empty << "\t" << executeUnit.opcode << "\t" << executeUnit.PCValue << "\tcompleted phase? " << completedExecute << "\t" << (!executeUnit.empty ? cpu->program->at(executeUnit.PCValue)->rawText : "none") << std::endl;
        std::cout << "M :: "
                  << "empty: " << memoryAccessUnit.empty << "\t" << memoryAccessUnit.opcode << "\t" << memoryAccessUnit.PCValue << "\tcompleted phase? " << completedMemoryAccess << "\t" << (!memoryAccessUnit.empty ? cpu->program->at(memoryAccessUnit.PCValue)->rawText : "none") << std::endl;
        std::cout << "W :: "
                  << "empty: " << writebackUnit.empty << "\t" << writebackUnit.opcode << "\t" << writebackUnit.PCValue << "\tcompleted phase? " << completedWriteback << "\t" << (!writebackUnit.empty ? cpu->program->at(writebackUnit.PCValue)->rawText : "none") << std::endl;
    }

    return pipelineAdvanced;
}

// SuperscalarPipeline::SuperscalarPipeline(int n)
// {
//     if (n <= 1)
//         printf("Warning: A superscalar pipeline should have n >= 1, consider using a scalar pipeline");

//     fetchUnits = (FetchUnit *)calloc(n, sizeof(FetchUnit));
//     decodeUnits = (DecodeUnit *)calloc(n, sizeof(DecodeUnit));
//     executeUnits = (ExecuteUnit *)calloc(n, sizeof(ExecuteUnit));
//     memoryAccessUnits = (MemoryAccessUnit *)calloc(n, sizeof(MemoryAccessUnit));
//     writebackUnits = (WriteBackUnit *)calloc(n, sizeof(WriteBackUnit));
// }

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

    if (debug)
        std::cout << "----------- Cycle #" << cycles << " :: PC:" << registers[PC] << "-----------" << std::endl;

    assert(running);

    // If the program counter value is greater than the size of the program, just send NOP forever
    // if (!(registers[PC] < (int32_t)program->size()))
    // {
    //     printf("PC beyond end of program on cycle %d\n", cycles); // debug
    //     program->push_back(new Instruction());                    // add NOP to end of program
    // }

    // Advance the pipeline
    bool pipelineAdvanced = pipeline->Advance(this, program, scoreboard);

    if (pipelineAdvanced)
        registers[PC]++;

    // If the pipeline advanced we can move the pc along
    // if (pipelineAdvanced)
    // {
    //     // Increment PC
    //     registers[PC]++;
    // }
    // else
    // {
    //     if (debug)
    //     {
    //         std::cout << "Didnt inc pc" << std::endl;
    //     }
    // }

    std::this_thread::sleep_for(std::chrono::milliseconds(speed));
}

void CPU::regDump()
{
    std::cout << "------------REG-DUMP-------------" << std::endl;
    for (int i = 0; i < REGABI_UNUSED; i++)
    {
        std::cout << getStringFromRegName((REGISTER_ABI_NAME)i) << ":\t" << registers[i] << std::endl;
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