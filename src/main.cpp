
// We shouldnt build this in test mode as tests will have their own main()
#include <string>
#include <getopt.h>
#include <stdio.h>
#include <string.h>

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
extern int bp_mode;

int main(int argc, char *const argv[])
{
    // Handle command line arguments
    int cpu_speed;
    int memSize = 128;
    int numDecoders = 1;
    int numAdders = 1;
    int numLoadStores = 1;
    int robSize = 16;

    int opt;
    std::string filename;

    int option_index = 0;
    static struct option long_options[] = {{"mode", required_argument, NULL, 0}, {"program", required_argument, NULL, 0}, {"debug", no_argument, NULL, 0}, {"step", no_argument, NULL, 0}, {"bp", required_argument, NULL, 0}};

    while ((opt = getopt_long(argc, argv, "s:m:a:d:l:", long_options, &option_index)) != -1)
    {

        switch (opt)
        {
        case 0:
            // LONG ARGUMENTS  Switch by index
            switch (option_index)
            {
            case 0: // mode

                if (strcmp(optarg, "ooo") == 0)
                {
                    mode = TOMASULOS;
                }
                else if (strcmp(optarg, "scalar") == 0)
                {
                    mode = SCALAR;
                }
                break;
            case 1: // program (filepath)
                filename = optarg;
                break;
            case 2: // debug
                debug = true;
                break;
            case 3: // step
                step = true;
                break;
            case 4: // bp mode
                if (strcmp(optarg, "2BIT") == 0)
                {
                    bp_mode = TWO_BIT;
                }
                else if (strcmp(optarg, "AT") == 0)
                {
                    bp_mode = ALWAYS_TAKE;
                }
                else if (strcmp(optarg, "NT") == 0)
                {
                    bp_mode = NEVER_TAKE;
                }
                break;

            default:
                break;
            }
            break;
        case 's':
            cpu_speed = atoi(optarg);
            break;
        case 'm':
            memSize = atoi(optarg);
            break;
        case 'd':
            numDecoders = atoi(optarg);
            break;
        case 'a':
            numAdders = atoi(optarg);
            break;
        case 'l':
            numLoadStores = atoi(optarg);
            break;
        case 'r':
            robSize = atoi(optarg);
        }
    }

    // Parse
    runnable_program *program = new runnable_program;
    std::vector<int32_t> *data = new std::vector<int32_t>();

    bool parsingSuccess = parse(filename, program, data);
    if (!parsingSuccess)
    {
        exit(EXIT_FAILURE);
    }

    if (data->size() > memSize)
    {
        std::cout << TERMINAL_RED << "NOT ENOUGH MEMORY FOR DATA SEGMENT" << TERMINAL_RESET << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << TERMINAL_GREEN << "Successfully Read:\t" << filename.c_str() << TERMINAL_RESET << std::endl;
    std::cout << TERMINAL_BLUE << "Mode:\t" << ((mode == TOMASULOS) ? "ooo" : "scalar") << TERMINAL_RESET << std::endl;
    std::cout << TERMINAL_YELLOW << "Memory Size: " << memSize << TERMINAL_RESET << std::endl;
    std::cout << TERMINAL_CYAN << "Debug Mode:\t" << ((debug) ? "Enabled" : "Disabled") << TERMINAL_RESET << std::endl;
    std::cout << TERMINAL_MAGENTA << "Step mode:\t" << ((step) ? "Enabled" : "Disabled") << TERMINAL_RESET << std::endl;
    std::cout << TERMINAL_BLUE << "BP mode:\t" << bp_mode << TERMINAL_RESET << std::endl;
    std::cout << TERMINAL_BOLD_YELLOW << "UNITS:\t"
              << "Decoders: " + std::to_string(numDecoders) << "\t"
              << "ALUs: " + std::to_string(numAdders) << "\t\t"
              << "Load/Store units: " + std::to_string(numLoadStores) << "\t" << TERMINAL_RESET << std::endl;

    if (mode == SCALAR)
    {
        Pipeline *pipeline = new ScalarPipeline();
        Scoreboard *scoreboard = new SimpleScoreboard();

        CPU *cpu = new CPU(pipeline, scoreboard, memSize, cpu_speed);
        pipeline->setCPU(cpu);

        cpu->LoadProgram(program);

        int32_t memoryIndex = 0;
        for (auto &word : *data)
        {
            cpu->memory[memoryIndex] = word;
            memoryIndex++;
        }

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
        TomasulosCPU *cpu = new TomasulosCPU(program, data, memSize, numDecoders, numAdders, numLoadStores, bp_mode, robSize);

        cpu->Run(cpu_speed, step);
    }
    else
    {
        std::cout << "UNKNOWN MODE - exiting" << std::endl;
    }

    return 0;
}
