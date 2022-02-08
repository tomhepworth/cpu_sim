#include "isa.h"
#include <stdio.h>
#include "parser.h"
#include <string>
#include "main.h"

#define MEMORY_SIZE 65536 // Memory size in WORDS

static int32_t memory[MEMORY_SIZE];
bool running = true;
int cycles = 0;
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

    printf("Running...\n");

    registers[PC] = 0;
    while (running)
    {
        cycles++;
        // printf("Cycle %d\n", cycles);
        // Fetch
        Instruction i = program[registers[PC]];

        // Decode already done

        // Execute
        i.Execute();

        registers[PC] += 1;
    }

    printf("Program finished executing in %d cycles. T0 was %d\n", cycles, registers[T0]);

    return 0;
}
