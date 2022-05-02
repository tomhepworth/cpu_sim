#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"

#include "cpu.h"
#include "isa.h"
#include "parser.h"
#include "tomasulo.hpp"
#include "debug_utils.h"

#include <string>
#include <iostream>

std::string TEST_PROGRAM_PATH = "src/tests/test_programs/";

CPU *setupCPU(std::string filename, int memSize)
{
    // debug = true;
    Pipeline *pipeline = new ScalarPipeline();
    Scoreboard *scoreboard = new SimpleScoreboard();
    CPU *cpu = new CPU(pipeline, scoreboard, memSize, 1);
    pipeline->setCPU(cpu);

    runnable_program *program = new runnable_program;
    std::cout << (TEST_PROGRAM_PATH + filename) << std::endl;

    bool parsingSuccess = parse(TEST_PROGRAM_PATH + filename, program);
    if (!parsingSuccess)
    {
        exit(EXIT_FAILURE);
    }

    cpu->LoadProgram(program);
    cpu->registers[PC] = 0;
    std::cout << "CPU set up..." << cpu->getMemorySize() << std::endl;
    return cpu;
}

// Return program until it halts or exits
void runCPU(CPU *cpu)
{
    std::cout << "Running..." << std::endl;
    while (cpu->running)
    {
        cpu->Cycle();
        cpu->cycles++;
    }
    std::cout << "Finished..." << std::endl;
}

TEST_CASE("Can set up CPU with simple program", "[cpu]")
{
    std::string test_program = "hlt.prog";
    CPU *cpu = setupCPU(test_program, 64);
    runCPU(cpu);

    REQUIRE(cpu->running == false);
    REQUIRE(cpu->getMemorySize() == 64);

    // Test memory wasnt changed
    for (int memAddr = 0; memAddr < cpu->getMemorySize(); memAddr++)
    {
        REQUIRE(cpu->memory[memAddr] == 0);
    }

    // Test registers (other than PC) werent changed
    for (int i = 0; i < REG_UNUSED; i++)
    {
        if (i == PC)
            continue;

        if (cpu->registers[i] != 0)
            std::cout << "REG WASNT ZZERO ::" << cpu->registers[i] << "REG: " << getStringFromRegName((REGISTER_ABI_NAME)i) << std::endl;

        REQUIRE(cpu->registers[i] == 0);
    }
}

TEST_CASE("Add two immediates and save in t0", "[cpu]")
{
    std::string test_program = "add.prog";
    CPU *cpu = setupCPU(test_program, 16);
    runCPU(cpu);

    REQUIRE(cpu->running == false);
    REQUIRE(cpu->registers[T0] == 8);
}

TEST_CASE("Multiply test: uses loops, loads, stores, offset syntax", "[cpu]")
{
    std::string test_program = "mul.prog";
    CPU *cpu = setupCPU(test_program, 512);
    runCPU(cpu);

    REQUIRE(cpu->running == false);
    REQUIRE(cpu->registers[T3] == 35);
    REQUIRE(cpu->memory[0] == 35);
}

TEST_CASE("Loads and stores with different syntax", "[cpu]")
{
    std::string test_program = "loadStoreTest.prog";
    CPU *cpu = setupCPU(test_program, 512);

    REQUIRE(cpu->registers[T0] == 0);
    REQUIRE(cpu->registers[T1] == 0);
    REQUIRE(cpu->registers[T2] == 0);
    REQUIRE(cpu->registers[T3] == 0);
    REQUIRE(cpu->memory[0] == 0);
    REQUIRE(cpu->memory[4] == 0);

    runCPU(cpu);

    REQUIRE(cpu->running == false);

    REQUIRE(cpu->registers[T0] == 100);
    REQUIRE(cpu->registers[T1] == 200);
    REQUIRE(cpu->registers[T2] == 100);
    REQUIRE(cpu->registers[T3] == 200);
    REQUIRE(cpu->memory[0] == 100);
    REQUIRE(cpu->memory[4] == 200);
}

TEST_CASE("Relative branching", "[cpu]")
{
    std::string test_program = "relativeBranchTest.prog";
    CPU *cpu = setupCPU(test_program, 512);

    runCPU(cpu);

    REQUIRE(cpu->running == false);

    REQUIRE(cpu->registers[T0] == 5);
    REQUIRE(cpu->registers[T1] == 5);
}

TEST_CASE("Test reservation station tags", "[Reservation Stations]")
{
    CommonDataBus *cdb = new CommonDataBus();
    ReservationStationTable *t = new ReservationStationTable();
    DistributedReservationStation *drs = new DistributedReservationStation(t, 3, "test", ADDER, cdb);

    REQUIRE(drs->masterTag == "test");
    REQUIRE(drs->stations[0]->tag == "test0");
    REQUIRE(drs->stations[1]->tag == "test1");
    REQUIRE(drs->stations[2]->tag == "test2");
}

TEST_CASE("Tomasulo Decoder Tests", "[Tomasulo Decoder]")
{
    // Set up
    runnable_program *p = new runnable_program;
    bool parsingSuccess = parse(TEST_PROGRAM_PATH + "tomasuloTest1.prog", p);
    assert(parsingSuccess);

    ReservationStationTable *reservationStationTable = new ReservationStationTable();
    RegisterStatusTable *registerStatusTable = new RegisterStatusTable();
    CommonDataBus *cdb = new CommonDataBus(reservationStationTable, registerStatusTable);
    registerStatusTable->cdb = cdb; // messy but necessary

    ReorderBuffer *rob = new ReorderBuffer(16, cdb);

    DistributedReservationStation *loadstoreRS = new DistributedReservationStation(reservationStationTable, 1, "LOAD_STORE", LOAD_STORE, cdb);
    DistributedReservationStation *adderRS = new DistributedReservationStation(reservationStationTable, 2, "ADDER", ADDER, cdb);

    TomasulosDecoder *decoder = new TomasulosDecoder(p, registerStatusTable, reservationStationTable, rob);

    // 1 cycle should take and pass to adder unit's ReservationStation
    decoder->Cycle();
    REQUIRE(decoder->stalled == false);

    // Check decoder was populated properly
    REQUIRE(decoder->opcode == ADD);
    REQUIRE(decoder->rd == T3);
    REQUIRE(decoder->rs1 == T1);
    REQUIRE(decoder->rs2 == T2);
    REQUIRE(decoder->imm == 0);

    // Check value was passed on to adder reservation station
    ReservationStation *rs = reservationStationTable->findByTag(decoder->lastUpdatedRS);
    TAG tag1 = decoder->lastUpdatedRS;

    std::cout << " (1) last updated tag was: " << decoder->lastUpdatedRS << std::endl;
    assert(rs != nullptr);

    REQUIRE(rs->val1 == 0);
    REQUIRE(rs->val2 == 0);
    REQUIRE(rs->source1 == "");
    REQUIRE(rs->source2 == "");

    // Mock PC increasing after first cycle
    registerStatusTable->setRegValue(PC, 1);

    // Cycle 2, next instruction has dependencies
    decoder->Cycle();
    REQUIRE(decoder->stalled == false);

    // Check decoder was populated properly
    REQUIRE(decoder->opcode == ADD);
    REQUIRE(decoder->rd == T5);
    REQUIRE(decoder->rs1 == T3);
    REQUIRE(decoder->rs2 == T4);
    REQUIRE(decoder->imm == 0);

    // Check value was passed on to adder reservation station
    ReservationStation *rs2 = reservationStationTable->findByTag(decoder->lastUpdatedRS);
    std::cout << " (2) last updated tag was: " << decoder->lastUpdatedRS << std::endl;
    assert(rs2 != nullptr);

    REQUIRE(rs2->val1 == 0);
    REQUIRE(rs2->val2 == 0);
    REQUIRE(rs2->source1 == tag1);
    REQUIRE(rs2->source2 == "");

    // Mock PC increasing after seoond cycle
    registerStatusTable->setRegValue(PC, 2);
    decoder->Cycle();
    REQUIRE(decoder->stalled == true); // Should now stall as no room for 3rd add in RS
}

TEST_CASE("Test Tomasulos Adder", "[Tomasulos Adder]")
{
    std::cout << "AAAAA" << std::endl;

    runnable_program *p = new runnable_program;
    bool parsingSuccess = parse(TEST_PROGRAM_PATH + "tomasuloTest1.prog", p);
    assert(parsingSuccess);

    ReservationStationTable *reservationStationTable = new ReservationStationTable();
    RegisterStatusTable *registerStatusTable = new RegisterStatusTable();
    CommonDataBus *cdb = new CommonDataBus(reservationStationTable, registerStatusTable);
    registerStatusTable->cdb = cdb; // messy but necessary

    ReorderBuffer *rob = new ReorderBuffer(16, cdb);

    AdderUnit *adderFU = new AdderUnit(cdb, reservationStationTable, "ADDER", rob, ADDER);

    TomasulosDecoder *decoder = new TomasulosDecoder(p, registerStatusTable, reservationStationTable, rob);

    REQUIRE(reservationStationTable->findByTag("ADDER0")->valid == false); // Should not be valid before RS is populated

    // 1 cycle should take and pass to adder unit's ReservationStation
    decoder->Cycle();
    // Quickly confirm
    REQUIRE(decoder->stalled == false);
    REQUIRE(decoder->opcode == ADD);

    // Check register status was updated
    REQUIRE(registerStatusTable->entries.at(T3)->valid == false);
    REQUIRE(registerStatusTable->entries.at(T3)->tag == "ADDER0");

    // Check reservation station was updated
    REQUIRE(reservationStationTable->findByTag("ADDER0")->valid == true); // Should now be valid as ready to execute next cycle
    REQUIRE(reservationStationTable->findByTag("ADDER0")->empty == false);

    // Adders 1st cycle should get instruction from RS and execute
    adderFU->Cycle();
    REQUIRE(adderFU->opcode == ADD);
    REQUIRE(adderFU->toExecute->tag == "ADDER0");

    // Check it executed and got broadcast by CDB and put back in reg status table
    REQUIRE(registerStatusTable->entries.at(T3)->valid == true);
    REQUIRE(registerStatusTable->entries.at(T3)->tag == "");

    // Check it executed and got broadcast by CDB and put back in reservationStation table
    // REQUIRE(reservationStationTable->findByTag("ADDER0")->valid == false); //Perhaps shouldnt be the case? RS is cleared when passed to FU
    REQUIRE(reservationStationTable->findByTag("ADDER0")->empty == true);
}