#include "isa.h"
#include <stdio.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <getopt.h>

#include "parser.h"
#include <string>
#include "main.h"
#include "isa.h"
#include "cpu.h"

#define MEMORY_SIZE 65536 // Memory size in WORDS

extern char *optarg;
extern int optind, opterr, optopt;

int32_t memory[MEMORY_SIZE];
bool running = true; // Global used for dealing with HLT instruction
bool debug = false;  // global debug
bool step = false;   // stepthrough mode
int cpu_speed = 10;  // default 10ms delay between cycles;

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

int main(int argc, char *const argv[])
{
    // Handle command line arguments
    int opt;
    std::string filename;
    while ((opt = getopt(argc, argv, "-:s:dt")) != -1)
    {
        switch (opt)
        {
        case 1:
            printf("Path: %s\n", optarg);
            filename = optarg;
            break;
        case 's':
            printf("Speed: %s\n", optarg);
            cpu_speed = atoi(optarg);
            break;
        case 'd':
            printf("Debug Enabled\n");
            debug = true;
            break;
        case 't':
            printf("Step mode enabled\n");
            step = true;
        }
    }

    // Parse
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
        if (step) // If in step-through mode, wait for stdin
        {
            char input;
            do
            {
                std::cout << '\n'
                          << "Press a key to step...";
                input = std::cin.get();

                if (input == 'r')
                    regDump();

                if (input == 'm')
                    memDump(0, 15);

                if (input == 's')
                    scoreboard->log();

                if (input == 'c') // continue execution without step mode
                    step = false;

            } while (input != '\n');
        }

        cycles++;
        cpu.Cycle();
        std::this_thread::sleep_for(std::chrono::milliseconds(cpu_speed));
    }

    printf("Program finished executing in %d cycles. T0 was %d\n", cpu.getCycles(), registers[T0]);

    regDump();
    memDump(0, 10);

    return 0;
}
