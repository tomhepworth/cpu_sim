#include "tomasulo.hpp"
#include "debug_utils.h"

// TomasulosCPU::TomasulosCPU()
// {
// }

TomasulosCPU::TomasulosCPU(runnable_program *prog)
{
    cycles = 0;
    program = prog;

    reservationStationTable = new ReservationStationTable();
    registerStatusTable = new RegisterStatusTable();
    cdb = new CommonDataBus(reservationStationTable, registerStatusTable);
    registerStatusTable->cdb = cdb; // messy but necessary
    rob = new ReorderBuffer(16, cdb);

    // Create functional units
    adder = new AdderUnit(cdb, reservationStationTable, "ADDER", rob, ADDER);
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
}

bool TomasulosCPU::Cycle()
{
    bool halt = false;
    rob->Cycle();
    adder->Cycle();
    bool decodeStalled = decoder->Cycle();

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
        std::cout << "------- Decoder: " << std::endl;
        decoder->print();
        std::cout << "------- Adder: " << std::endl;
        adder->print();
        std::cout << "------- RegStatus: " << std::endl;
        registerStatusTable->print();
        std::cout << "------- ResStation: " << std::endl;
        reservationStationTable->print();

        std::cout << "------- ROB: " << std::endl;
        rob->print();

        std::cout
            << " =========== END TOMASULOS CYCLE " << cycles << " PC: " << registerStatusTable->getRegValue(PC) << "  ========== " << std::endl;
    }

    cycles++;

    return halt;
}