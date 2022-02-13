#include "isa.h"
#include <stdio.h>
#include <iostream>
#include <chrono>
#include <thread>

#include "parser.h"
#include <string>
#include "main.h"
#include "cpu.h"

#define MEMORY_SIZE 65536 // Memory size in WORDS

static int32_t memory[MEMORY_SIZE];
bool running = true;
int cycles = 0;

enum CPU_STAGE
{
    FETCH,
    DECODE,
    BLAH,
    EXECUTE,
    WRITEBACK
};

int main(int argc, char const *argv[])
{
    if (argc < 2)
    {
        printf("Enter path to a program\n");
        return 0;
    }

    std::string filename = argv[1];
    printf("Parsing: %s\n", filename.c_str());
    runnable_program program = parse(filename);

    Pipeline pipeline = Pipeline(program);
    CPU cpu = CPU(pipeline);

    cpu.LoadProgram(program);

    printf("Running...\n");

    // Todo:
    //  Pipeline fetch,decode,execute

    registers[PC] = 0;
    while (running)
    {
        cycles++;
        cpu.Cycle();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        // // Fetch & Decode
        // Instruction i = program[registers[PC]];

        // // Execute
        // i.Execute();

        // registers[PC] += 1;
    }

    printf("Program finished executing in %d cycles. T0 was %d\n", cycles, registers[T0]);

    return 0;
}
