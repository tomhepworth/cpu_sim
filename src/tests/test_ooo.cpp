#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "tomasulo.hpp"

#include <string>
#include <iostream>

std::string TEST_PROGRAM_PATH = "src/tests/test_programs/";

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

TEST_CASE("add.prog", "[ooo]")
{
    std::string test_program = TEST_PROGRAM_PATH + "add.prog";
    runnable_program *program = new runnable_program;

    bool parsingSuccess = parse(test_program, program);
    if (!parsingSuccess)
    {
        exit(EXIT_FAILURE);
    }
    TomasulosCPU *cpu = new TomasulosCPU(program, 256);

    cpu->Run(1, false);

    REQUIRE(cpu->registerStatusTable->getRegValue(T0) == 8);
}

TEST_CASE("mul.prog", "[ooo]")
{
    std::string test_program = TEST_PROGRAM_PATH + "mul.prog";
    runnable_program *program = new runnable_program;

    bool parsingSuccess = parse(test_program, program);
    if (!parsingSuccess)
    {
        exit(EXIT_FAILURE);
    }

    TomasulosCPU *cpu = new TomasulosCPU(program, 256);

    cpu->Run(1, false);

    REQUIRE(cpu->registerStatusTable->getRegValue(T3) == 35);
    REQUIRE(cpu->memory[0] == 35);
}

TEST_CASE("collatz.prog", "[ooo]")
{
    std::string test_program = TEST_PROGRAM_PATH + "collatz_test.prog";
    runnable_program *program = new runnable_program;

    bool parsingSuccess = parse(test_program, program);
    if (!parsingSuccess)
    {
        exit(EXIT_FAILURE);
    }

    TomasulosCPU *cpu = new TomasulosCPU(program, 256);

    cpu->Run(1, false);

    REQUIRE(cpu->memory[0] == 3);
    REQUIRE(cpu->memory[1] == 10);
    REQUIRE(cpu->memory[2] == 5);
    REQUIRE(cpu->memory[3] == 16);
    REQUIRE(cpu->memory[4] == 8);
    REQUIRE(cpu->memory[5] == 4);
    REQUIRE(cpu->memory[6] == 2);
    REQUIRE(cpu->memory[7] == 1);
}

TEST_CASE("loadStoreTest.prog", "[ooo]")
{
    std::string test_program = TEST_PROGRAM_PATH + "loadStoreTest.prog";
    runnable_program *program = new runnable_program;

    bool parsingSuccess = parse(test_program, program);
    if (!parsingSuccess)
    {
        exit(EXIT_FAILURE);
    }

    TomasulosCPU *cpu = new TomasulosCPU(program, 256);

    cpu->Run(1, false);

    REQUIRE(cpu->memory[0] == 100);
    REQUIRE(cpu->memory[4] == 200);
    REQUIRE(cpu->memory[10] == 100);
    REQUIRE(cpu->physicalRegisters[T0] == 100);
    REQUIRE(cpu->physicalRegisters[T1] == 200);
    REQUIRE(cpu->physicalRegisters[T2] == 100);
    REQUIRE(cpu->physicalRegisters[T3] == 200);
}

TEST_CASE("relativeBranchTest.prog", "[ooo]")
{
    std::string test_program = TEST_PROGRAM_PATH + "relativeBranchTest.prog";
    runnable_program *program = new runnable_program;

    bool parsingSuccess = parse(test_program, program);
    if (!parsingSuccess)
    {
        exit(EXIT_FAILURE);
    }

    TomasulosCPU *cpu = new TomasulosCPU(program, 256);

    cpu->Run(1, false);
    REQUIRE(cpu->physicalRegisters[T0] == 5);
    REQUIRE(cpu->physicalRegisters[T1] == 5);
}