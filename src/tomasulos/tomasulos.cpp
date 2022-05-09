#include "tomasulo.hpp"
#include "debug_utils.h"

// TomasulosCPU::TomasulosCPU()
// {
// }

TomasulosCPU::TomasulosCPU(runnable_program *prog, int32_t _memorySize)
{
    cycles = 0;
    program = prog;
    memorySize = _memorySize;
    memory = (int32_t *)calloc(memorySize, sizeof(int32_t));

    reservationStationTable = new ReservationStationTable();
    registerStatusTable = new RegisterStatusTable();
    cdb = new CommonDataBus(reservationStationTable, registerStatusTable);
    registerStatusTable->cdb = cdb; // messy but necessary
    rob = new ReorderBuffer(16, cdb, memory);

    // Create functional units
    adder = new AdderUnit(memory, cdb, reservationStationTable, "ADDER", rob, ADDER);
    loadStoreUnit = new LoadStoreUnit(memory, cdb, reservationStationTable, "LS", rob, LOAD_STORE);

    decoder = new TomasulosDecoder(program, registerStatusTable, reservationStationTable, rob);

    std::cout << "TomasulosCPU halted" << std::endl;
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

        if (halt)
            break;
    }

    // TODO Print pretty stats here
    std::cout << TERMINAL_RED << "Program Halting!" << TERMINAL_RESET << std::endl;
    std::cout << TERMINAL_CYAN << "===== " << TERMINAL_MAGENTA << "Stats!" << TERMINAL_CYAN << " =====" << TERMINAL_RESET << std::endl;
    std::cout << TERMINAL_GREEN << "Cycles:\t" << cycles << TERMINAL_RESET << std::endl;
    std::cout << TERMINAL_RED << "Stalls:\t" << cycles << TERMINAL_RESET << std::endl;
    std::cout << TERMINAL_BLUE << "IPC:\t" << mean_ipc << TERMINAL_RESET << std::endl;

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
    int32_t currentPCValue = registerStatusTable->getRegValue(PC);
    if (!decodeStalled && currentPCValue < program->size() - 1)
    {
        registerStatusTable->setRegValue(PC, currentPCValue + 1);
    }

    if (debug)
    {
        std::cout << " ============= TOMASULOS CYCLE " << cycles << " PC: " << registerStatusTable->getRegValue(PC) << " =========================================================================" << std::endl;

        if (decodeStalled)
        {
            std::cout << TERMINAL_RED << "Stalled!!" << TERMINAL_RESET << std::endl;
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

        std::cout
            << TERMINAL_RESET << " =========== END TOMASULOS CYCLE " << cycles << " PC: " << registerStatusTable->getRegValue(PC) << "  ========== " << std::endl;
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
            std::cout << " |\t0x" << i + j * columnsize << "\t" << memory[i + j * columnsize];
        }
        std::cout << std::endl;
    }
}