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
    adder = new AdderUnit(cdb, reservationStationTable, "ADDER", ADDER);
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
    decoder->Cycle();
    adder->Cycle();

    if (debug)
    {
        std::cout << " ============= TOMASULOS CYCLE " << cycles << " PC: " << registerStatusTable->getRegValue(PC) << " ============ " << std::endl;
        std::cout << "------- RegStatus: " << std::endl;
        registerStatusTable->print();
        std::cout << "------- ResStation: " << std::endl;
        reservationStationTable->print();
        std::cout << "------- Decoder: " << std::endl;
        decoder->print();
        std::cout << "------- Adder: " << std::endl;
        adder->print();
        std::cout << "------- ROB: " << std::endl;
        rob->print();

        std::cout
            << " =========== END TOMASULOS CYCLE " << cycles << " PC: " << registerStatusTable->getRegValue(PC) << "  ========== " << std::endl;
    }

    cycles++;

    return halt;
}