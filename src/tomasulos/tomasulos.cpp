#include "tomasulo.hpp"
#include "debug_utils.h"

// TomasulosCPU::TomasulosCPU()
// {
// }

TomasulosCPU::TomasulosCPU(runnable_program *prog, std::vector<int32_t> *data, int32_t _memorySize)
{
    stalls = 0;
    mean_ipc = 0;
    total_executed_instructions = 0;
    cycles = 0;
    program = prog;
    memorySize = _memorySize;
    memory = (int32_t *)calloc(memorySize, sizeof(int32_t));
    physicalRegisters = (int32_t *)calloc(REGABI_UNUSED, sizeof(int32_t));

    reservationStationTable = new ReservationStationTable();
    registerStatusTable = new RegisterStatusTable();
    cdb = new CommonDataBus(reservationStationTable, registerStatusTable);
    registerStatusTable->cdb = cdb; // messy but necessary
    registerStatusTable->physicalRegisters = physicalRegisters;

    rob = new ReorderBuffer(16, cdb, memory, physicalRegisters, this);

    // Create functional units
    adder = new AdderUnit(physicalRegisters, memory, cdb, reservationStationTable, "ADDER", rob, ADDER);
    loadStoreUnit = new LoadStoreUnit(physicalRegisters, memory, cdb, reservationStationTable, "LS", rob, LOAD_STORE);
    decoder = new TomasulosDecoder(program, registerStatusTable, reservationStationTable, rob, physicalRegisters);

    // Copy data into memory
    int32_t memoryIndex = 0;
    for (auto &word : *data)
    {
        memory[memoryIndex] = word;
        memoryIndex++;
    }

    std::cout << "Mem 0 is" << memory[0] << std::endl;
}

void TomasulosCPU::Run(int speed, bool step)
{
    // Main loop
    while (true)
    {
        if (step) // If in step-through mode, wait for stdin
        {
            char input;
            do
            {
                std::cout << '\n'
                          << "Press a key to step..." << std::endl;
                input = std::cin.get();
            } while (input != '\n');
        }

        bool halt = Cycle();

        // Record Stats
        total_executed_instructions += rob->committedThisCycle;

        if (halt)
            break;
    }

    IF_DEBUG(std::cout << TERMINAL_RED << "Program Halting!" << TERMINAL_RESET << std::endl);

    // Final stats calculations
    mean_ipc = (float)total_executed_instructions / (float)cycles;

    std::cout << TERMINAL_BOLD_CYAN << "===== " << TERMINAL_BOLD_GREEN << "Done!" << TERMINAL_CYAN << " =====" << TERMINAL_RESET << std::endl;
    std::cout << TERMINAL_MAGENTA << "Stats... " << TERMINAL_RESET << std::endl;
    std::cout << TERMINAL_BOLD_GREEN << "Cycles:\t" << TERMINAL_GREEN << cycles << TERMINAL_RESET << std::endl;
    std::cout << TERMINAL_BOLD_RED << "Stalls:\t" << TERMINAL_RED << stalls << TERMINAL_RESET << std::endl;
    std::cout << TERMINAL_BOLD_BLUE << "IPC:\t" << TERMINAL_BLUE << mean_ipc << TERMINAL_RESET << std::endl;

    return;
}

bool TomasulosCPU::Cycle()
{
    bool halt = false;
    rob->Cycle();
    adder->Cycle();
    loadStoreUnit->Cycle();
    bool decodeStalled = decoder->Cycle(cycles);

    // Keep track of how many cycles we stall on
    if (decodeStalled)
        stalls++;

    // If decode didnt stall then it's safe to increase the PC
    // Dont increase if we have reached the end of the program
    int32_t currentPCValue = physicalRegisters[PC];
    if (!decodeStalled && currentPCValue < program->size() - 1)
    {
        registerStatusTable->setRegValue(PC, currentPCValue + 1);
        physicalRegisters[PC] = currentPCValue + 1;
    }

    if (debug)
    {
        std::cout << " ============= TOMASULOS CYCLE " << cycles << " PC: " << registerStatusTable->getRegValue(PC) << "  PC PHYSICAL: " << physicalRegisters[PC] << " =========================================================================" << std::endl;

        if (decodeStalled)
        {
            std::cout << TERMINAL_RED << "Stalled!! Reason:" << stallReason << TERMINAL_RESET << std::endl;
        }

        std::cout << "------- Decoder: " << std::endl;
        decoder->print();
        std::cout << TERMINAL_RESET << "------- Adder: " << TERMINAL_GREEN << std::endl;
        adder->print();
        std::cout << TERMINAL_RESET << "------- LoadStore: " << TERMINAL_MAGENTA << std::endl;
        loadStoreUnit->print();
        std::cout << TERMINAL_RESET << "------- RegStatus: " << TERMINAL_CYAN << std::endl;
        registerStatusTable->print();
        std::cout << TERMINAL_RESET << "------- ResStation: " << TERMINAL_YELLOW << std::endl;
        reservationStationTable->print();
        std::cout << TERMINAL_RESET << "------- ROB: " << TERMINAL_BLUE << std::endl;
        rob->print();
        std::cout << TERMINAL_RESET << "------- MEMORY: " << TERMINAL_RED << std::endl;
        memDump();
        std::cout << TERMINAL_RESET << std::endl;
    }

    cycles++;

    if (decodeStalled && rob->count == 0 && reservationStationTable->allStationsEmpty())
    {
        halt = true; // TODO print with nice shutdown statistics
    }

    return halt;
}

void TomasulosCPU::memDump()
{
    int32_t cols = 12;
    int32_t columnsize = memorySize / cols;

    for (int i = 0; i < columnsize; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            std::cout << "| x" << i + j * columnsize << "\t" << memory[i + j * columnsize] << "\t";
        }
        std::cout << std::endl;
    }
}

void TomasulosCPU::flush()
{
    // Triggered by ROB, which flushes itself

    // Flush all FU's
    adder->flush();
    loadStoreUnit->flush();
    decoder->flush();

    // Flush all reservation stations
    reservationStationTable->flush();

    // Remove all tags from TST and reset values to their physical ones
    registerStatusTable->flush();

    return;
}