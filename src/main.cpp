#include "isa.h"
#include <stdio.h>
#include "parser.h"
#include <string>

int main(int argc, char const *argv[])
{
    if (argc < 2)
    {
        printf("Enter path to a program\n");
        return 0;
    }

    std::string filename = argv[1];
    printf("Running: %s\n", filename.c_str());

    program p = parse(filename);

    registers[RA] = 2;
    registers[SP] = 3;

    printf("REG: %d\n", registers[GP]);
    p[0].Execute();
    printf("REG: %d\n", registers[GP]);

    return 0;
}
