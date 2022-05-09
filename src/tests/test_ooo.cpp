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

TEST_CASE("add.prog", "[scalar-ooo]")
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

TEST_CASE("mul.prog", "[scalar-ooo]")
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