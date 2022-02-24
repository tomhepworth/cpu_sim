#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"

#include "cpu.h"
#include "isa.h"
#include "parser.h"

#include <string>
#include <iostream>

std::string TEST_PROGRAM_PATH = "src/tests/test_programs/";

CPU *setupCPU(std::string filename, int memSize)
{
    Pipeline *pipeline = new Pipeline();
    Scoreboard *scoreboard = new SimpleScoreboard();
    CPU *cpu = new CPU(pipeline, scoreboard, memSize, 1);

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
        cpu->cycles++;
        cpu->Cycle();
    }
    std::cout << "Finished..." << std::endl;
}

TEST_CASE("Can set up CPU with simple program", "[cpu]")
{
    std::string test_program = "hlt.prog";
    CPU *cpu = setupCPU(test_program, 512);
    runCPU(cpu);

    REQUIRE(cpu->running == false);
    REQUIRE(cpu->getMemorySize() == 512);

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

        REQUIRE(cpu->registers[i] == 0);
    }
}

TEST_CASE("Add two immediates and save in t0", "[cpu]")
{
    std::string test_program = "add.prog";
    CPU *cpu = setupCPU(test_program, 512);
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
