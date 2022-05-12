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
    std::vector<int32_t> *data = new std::vector<int32_t>();
    runnable_program *program = new runnable_program;

    bool parsingSuccess = parse(test_program, program, data);
    if (!parsingSuccess)
    {
        exit(EXIT_FAILURE);
    }
    TomasulosCPU *cpu = new TomasulosCPU(program, data, 256, 1, 1, 1);
    TomasulosCPU *cpu1 = new TomasulosCPU(program, data, 256, 1, 2, 1);
    TomasulosCPU *cpu2 = new TomasulosCPU(program, data, 256, 2, 1, 1);
    TomasulosCPU *cpu3 = new TomasulosCPU(program, data, 256, 2, 2, 1);

    cpu->Run(1, false);
    REQUIRE(cpu->registerStatusTable->getRegValue(T0) == 8);

    cpu1->Run(1, false);
    REQUIRE(cpu1->registerStatusTable->getRegValue(T0) == 8);

    cpu2->Run(1, false);
    REQUIRE(cpu2->registerStatusTable->getRegValue(T0) == 8);

    cpu3->Run(1, false);
    REQUIRE(cpu3->registerStatusTable->getRegValue(T0) == 8);
}

TEST_CASE("mul.prog", "[ooo]")
{
    std::string test_program = TEST_PROGRAM_PATH + "mul.prog";
    std::vector<int32_t> *data = new std::vector<int32_t>();
    runnable_program *program = new runnable_program;

    bool parsingSuccess = parse(test_program, program, data);
    if (!parsingSuccess)
    {
        exit(EXIT_FAILURE);
    }

    TomasulosCPU *cpu = new TomasulosCPU(program, data, 256, 1, 1, 1);
    TomasulosCPU *cpu1 = new TomasulosCPU(program, data, 256, 1, 2, 1);
    TomasulosCPU *cpu2 = new TomasulosCPU(program, data, 256, 2, 1, 1);
    TomasulosCPU *cpu3 = new TomasulosCPU(program, data, 256, 2, 2, 1);

    cpu->Run(1, false);
    REQUIRE(cpu->registerStatusTable->getRegValue(T3) == 35);
    REQUIRE(cpu->memory[0] == 35);
    cpu1->Run(1, false);
    REQUIRE(cpu1->registerStatusTable->getRegValue(T3) == 35);
    REQUIRE(cpu1->memory[0] == 35);
    cpu2->Run(1, false);
    REQUIRE(cpu2->registerStatusTable->getRegValue(T3) == 35);
    REQUIRE(cpu2->memory[0] == 35);
    cpu3->Run(1, false);
    REQUIRE(cpu3->registerStatusTable->getRegValue(T3) == 35);
    REQUIRE(cpu3->memory[0] == 35);
}

TEST_CASE("collatz.prog", "[ooo]")
{
    std::string test_program = TEST_PROGRAM_PATH + "collatz_test.prog";
    std::vector<int32_t> *data = new std::vector<int32_t>();
    runnable_program *program = new runnable_program;

    bool parsingSuccess = parse(test_program, program, data);
    if (!parsingSuccess)
    {
        exit(EXIT_FAILURE);
    }

    TomasulosCPU *cpu = new TomasulosCPU(program, data, 256, 1, 1, 1);
    TomasulosCPU *cpu1 = new TomasulosCPU(program, data, 256, 1, 2, 1);
    TomasulosCPU *cpu2 = new TomasulosCPU(program, data, 256, 2, 1, 1);
    TomasulosCPU *cpu3 = new TomasulosCPU(program, data, 256, 2, 2, 1);

    cpu->Run(1, false);
    REQUIRE(cpu->memory[0] == 3);
    REQUIRE(cpu->memory[1] == 10);
    REQUIRE(cpu->memory[2] == 5);
    REQUIRE(cpu->memory[3] == 16);
    REQUIRE(cpu->memory[4] == 8);
    REQUIRE(cpu->memory[5] == 4);
    REQUIRE(cpu->memory[6] == 2);
    REQUIRE(cpu->memory[7] == 1);

    cpu1->Run(1, false);
    REQUIRE(cpu1->memory[0] == 3);
    REQUIRE(cpu1->memory[1] == 10);
    REQUIRE(cpu1->memory[2] == 5);
    REQUIRE(cpu1->memory[3] == 16);
    REQUIRE(cpu1->memory[4] == 8);
    REQUIRE(cpu1->memory[5] == 4);
    REQUIRE(cpu1->memory[6] == 2);
    REQUIRE(cpu1->memory[7] == 1);

    cpu2->Run(1, false);
    REQUIRE(cpu2->memory[0] == 3);
    REQUIRE(cpu2->memory[1] == 10);
    REQUIRE(cpu2->memory[2] == 5);
    REQUIRE(cpu2->memory[3] == 16);
    REQUIRE(cpu2->memory[4] == 8);
    REQUIRE(cpu2->memory[5] == 4);
    REQUIRE(cpu2->memory[6] == 2);
    REQUIRE(cpu2->memory[7] == 1);

    cpu3->Run(1, false);
    REQUIRE(cpu3->memory[0] == 3);
    REQUIRE(cpu3->memory[1] == 10);
    REQUIRE(cpu3->memory[2] == 5);
    REQUIRE(cpu3->memory[3] == 16);
    REQUIRE(cpu3->memory[4] == 8);
    REQUIRE(cpu3->memory[5] == 4);
    REQUIRE(cpu3->memory[6] == 2);
    REQUIRE(cpu3->memory[7] == 1);
}

TEST_CASE("loadStoreTest.prog", "[ooo]")
{
    std::string test_program = TEST_PROGRAM_PATH + "loadStoreTest.prog";
    runnable_program *program = new runnable_program;
    std::vector<int32_t> *data = new std::vector<int32_t>();

    bool parsingSuccess = parse(test_program, program, data);
    if (!parsingSuccess)
    {
        exit(EXIT_FAILURE);
    }

    TomasulosCPU *cpu = new TomasulosCPU(program, data, 256, 1, 1, 1);
    TomasulosCPU *cpu1 = new TomasulosCPU(program, data, 256, 1, 2, 1);
    TomasulosCPU *cpu2 = new TomasulosCPU(program, data, 256, 2, 1, 1);
    TomasulosCPU *cpu3 = new TomasulosCPU(program, data, 256, 2, 2, 1);

    cpu->Run(1, false);
    REQUIRE(cpu->memory[0] == 100);
    REQUIRE(cpu->memory[4] == 200);
    REQUIRE(cpu->memory[10] == 100);
    REQUIRE(cpu->physicalRegisters[T0] == 100);
    REQUIRE(cpu->physicalRegisters[T1] == 200);
    REQUIRE(cpu->physicalRegisters[T2] == 100);
    REQUIRE(cpu->physicalRegisters[T3] == 200);

    cpu1->Run(1, false);
    REQUIRE(cpu1->memory[0] == 100);
    REQUIRE(cpu1->memory[4] == 200);
    REQUIRE(cpu1->memory[10] == 100);
    REQUIRE(cpu1->physicalRegisters[T0] == 100);
    REQUIRE(cpu1->physicalRegisters[T1] == 200);
    REQUIRE(cpu1->physicalRegisters[T2] == 100);
    REQUIRE(cpu1->physicalRegisters[T3] == 200);

    cpu2->Run(1, false);
    REQUIRE(cpu2->memory[0] == 100);
    REQUIRE(cpu2->memory[4] == 200);
    REQUIRE(cpu2->memory[10] == 100);
    REQUIRE(cpu2->physicalRegisters[T0] == 100);
    REQUIRE(cpu2->physicalRegisters[T1] == 200);
    REQUIRE(cpu2->physicalRegisters[T2] == 100);
    REQUIRE(cpu2->physicalRegisters[T3] == 200);

    cpu3->Run(1, false);
    REQUIRE(cpu3->memory[0] == 100);
    REQUIRE(cpu3->memory[4] == 200);
    REQUIRE(cpu3->memory[10] == 100);
    REQUIRE(cpu3->physicalRegisters[T0] == 100);
    REQUIRE(cpu3->physicalRegisters[T1] == 200);
    REQUIRE(cpu3->physicalRegisters[T2] == 100);
    REQUIRE(cpu3->physicalRegisters[T3] == 200);
}

TEST_CASE("relativeBranchTest.prog", "[ooo]")
{
    std::string test_program = TEST_PROGRAM_PATH + "relativeBranchTest.prog";
    std::vector<int32_t> *data = new std::vector<int32_t>();
    runnable_program *program = new runnable_program;

    bool parsingSuccess = parse(test_program, program, data);
    if (!parsingSuccess)
    {
        exit(EXIT_FAILURE);
    }

    TomasulosCPU *cpu = new TomasulosCPU(program, data, 256, 1, 1, 1);
    TomasulosCPU *cpu1 = new TomasulosCPU(program, data, 256, 1, 2, 1);
    TomasulosCPU *cpu2 = new TomasulosCPU(program, data, 256, 2, 1, 1);
    TomasulosCPU *cpu3 = new TomasulosCPU(program, data, 256, 2, 2, 1);

    cpu->Run(1, false);
    REQUIRE(cpu->physicalRegisters[T0] == 5);
    REQUIRE(cpu->physicalRegisters[T1] == 5);

    cpu1->Run(1, false);
    REQUIRE(cpu1->physicalRegisters[T0] == 5);
    REQUIRE(cpu1->physicalRegisters[T1] == 5);

    cpu2->Run(1, false);
    REQUIRE(cpu2->physicalRegisters[T0] == 5);
    REQUIRE(cpu2->physicalRegisters[T1] == 5);

    cpu3->Run(1, false);
    REQUIRE(cpu3->physicalRegisters[T0] == 5);
    REQUIRE(cpu3->physicalRegisters[T1] == 5);
}

TEST_CASE("dataSegmentTest.prog", "[ooo]")
{
    std::string test_program = TEST_PROGRAM_PATH + "dataSegmentTest.prog";
    std::vector<int32_t> *data = new std::vector<int32_t>();
    runnable_program *program = new runnable_program;

    bool parsingSuccess = parse(test_program, program, data);
    if (!parsingSuccess)
    {
        exit(EXIT_FAILURE);
    }

    TomasulosCPU *cpu = new TomasulosCPU(program, data, 256, 1, 1, 1);
    TomasulosCPU *cpu1 = new TomasulosCPU(program, data, 256, 1, 2, 1);
    TomasulosCPU *cpu2 = new TomasulosCPU(program, data, 256, 2, 1, 1);
    TomasulosCPU *cpu3 = new TomasulosCPU(program, data, 256, 2, 2, 1);

    cpu->Run(1, false);
    REQUIRE(cpu->physicalRegisters[T0] == 0xFFFF);
    REQUIRE(cpu->physicalRegisters[T1] == 0x1234);
    REQUIRE(cpu->physicalRegisters[T2] == 0x11FF);
    REQUIRE(cpu->physicalRegisters[T3] == 0x23AB);
    REQUIRE(cpu->physicalRegisters[T4] == 0);

    cpu1->Run(1, false);
    REQUIRE(cpu1->physicalRegisters[T0] == 0xFFFF);
    REQUIRE(cpu1->physicalRegisters[T1] == 0x1234);
    REQUIRE(cpu1->physicalRegisters[T2] == 0x11FF);
    REQUIRE(cpu1->physicalRegisters[T3] == 0x23AB);
    REQUIRE(cpu1->physicalRegisters[T4] == 0);

    cpu2->Run(1, false);
    REQUIRE(cpu2->physicalRegisters[T0] == 0xFFFF);
    REQUIRE(cpu2->physicalRegisters[T1] == 0x1234);
    REQUIRE(cpu2->physicalRegisters[T2] == 0x11FF);
    REQUIRE(cpu2->physicalRegisters[T3] == 0x23AB);
    REQUIRE(cpu2->physicalRegisters[T4] == 0);

    cpu3->Run(1, false);
    REQUIRE(cpu3->physicalRegisters[T0] == 0xFFFF);
    REQUIRE(cpu3->physicalRegisters[T1] == 0x1234);
    REQUIRE(cpu3->physicalRegisters[T2] == 0x11FF);
    REQUIRE(cpu3->physicalRegisters[T3] == 0x23AB);
    REQUIRE(cpu3->physicalRegisters[T4] == 0);
}