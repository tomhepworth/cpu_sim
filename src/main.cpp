
// We shouldnt build this in test mode as tests will have their own main()
#include <string>
#include <getopt.h>
#include <stdio.h>
#include "main.h"
#include "isa.h"
#include "parser.h"
#include "cpu.h"
#include "scoreboard.h"
#include "tomasulo.hpp"
#include "debug_utils.h"

extern char *optarg;
extern int optind, opterr, optopt;
extern int mode;

int main(int argc, char *const argv[])
{
    // Handle command line arguments
    int cpu_speed;
    int memSize = 64;

    int opt;
    std::string filename;
    while ((opt = getopt(argc, argv, "-:s:m:dt")) != -1)
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
        case 'm':
            printf("Memory Size: %s\n", optarg);
            memSize = atoi(optarg);
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
    runnable_program *program = new runnable_program;

    bool parsingSuccess = parse(filename, program);
    if (!parsingSuccess)
    {
        exit(EXIT_FAILURE);
    }

    if (mode == SCALAR)
    {
        Pipeline *pipeline = new ScalarPipeline();
        Scoreboard *scoreboard = new SimpleScoreboard();

        CPU *cpu = new CPU(pipeline, scoreboard, memSize, cpu_speed);
        pipeline->setCPU(cpu);

        cpu->LoadProgram(program);

        if (debug)
        {
            printf("CPU Memory size is : %d\n", cpu->getMemorySize());
            printf("Using debug settings\n");
        }

        printf("Running...\n");

        cpu->registers[PC] = 0;

        while (cpu->running)
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
                        cpu->regDump();

                    if (input == 'm')
                        cpu->memDump(0, 15);

                    if (input == 's')
                    {
                        scoreboard->log();
                    }

                    if (input == 'c') // continue execution without step mode
                        step = false;

                } while (input != '\n');
            }

            cpu->Cycle();
            cpu->cycles++;
        }

        printf("Program finished executing in %d cycles", cpu->getCycles());

        cpu->regDump();
        cpu->memDump(0, cpu->getMemorySize());
    }
    else if (mode == TOMASULOS)
    {
        std::cout << "Mode: TOMASULOS" << std::endl;
        TomasulosCPU *cpu = new TomasulosCPU(program);

        cpu->Run(cpu_speed, step);
    }
    else
    {
        std::cout << "UNKNOWN MODE - exiting" << std::endl;
    }

    return 0;
}
