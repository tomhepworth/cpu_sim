#include "isa.h"
#include <stdio.h>
#include <iostream>
#include <chrono>
#include <thread>

#include "parser.h"
#include <string>
#include "main.h"
#include "isa.h"
#include "cpu.h"

#define MEMORY_SIZE 65536 // Memory size in WORDS

int32_t memory[MEMORY_SIZE];

bool running = true; // Global used for dealing with HTL instruction
bool debug = true;   // global debug

int cycles = 0;

void regDump()
{
    std::cout << "------------REG-DUMP-------------" << std::endl;
    for (size_t i = 0; i < REGABI_UNUSED; i++)
    {
        std::cout << i << ":\t" << registers[i] << std::endl;
    }
    std::cout << "---------------------------------" << std::endl;
}

void memDump(int start, int end)
{
    std::cout << "------------MEM-DUMP-------------" << std::endl;
    for (size_t i = start; i <= end; i++)
    {
        std::cout << "0x" << i << ":\t" << memory[i] << std::endl;
    }
    std::cout << "---------------------------------" << std::endl;
}

int main(int argc, char const *argv[])
{
    if (argc < 2)
    {
        printf("Enter path to a program\n");
        return 0;
    }

    std::string filename = argv[1];
    printf("Parsing: %s\n", filename.c_str());
    runnable_program program;

    bool parsingSuccess = parse(filename, &program);
    if (!parsingSuccess)
    {
        exit(EXIT_FAILURE);
    }

    Pipeline pipeline = Pipeline();
    Scoreboard *scoreboard = new SimpleScoreboard();

    CPU cpu = CPU(&pipeline, scoreboard);

    cpu.LoadProgram(&program);

    if (debug)
    {
        printf("Using debug settings\n");
    }

    printf("Running...\n");

    registers[PC] = 0;
    while (running)
    {
        cycles++;
        cpu.Cycle();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    printf("Program finished executing in %d cycles. T0 was %d\n", cpu.getCycles(), registers[T0]);

    regDump();
    memDump(0, 10);

    return 0;
}
