#include "scoreboard.h"
#include <iostream>

bool Scoreboard::isValid(REGISTER_ABI_NAME reg)
{
    return true;
}

void Scoreboard::log()
{
    std::cout << "Log not implemented for default scoreboard" << std::endl;
}

SimpleScoreboard::SimpleScoreboard()
{
    // Initialize valid to false;
    for (auto &&i : valid)
    {
        i = true;
    }
};

bool SimpleScoreboard::isValid(REGISTER_ABI_NAME reg)
{
    return valid[reg];
}

void SimpleScoreboard::setValid(REGISTER_ABI_NAME reg)
{
    valid[reg] = true;
}

void SimpleScoreboard::setInvalid(REGISTER_ABI_NAME reg)
{
    valid[reg] = false;
}

void SimpleScoreboard::log()
{
    std::cout << "--------------------------Scoreboard---------------------------" << std::endl;
    for (size_t i = 0; i < REGABI_UNUSED; i++)
    {
        std::cout << i << ":\t" << valid[i] << std::endl;
    }
    std::cout << "---------------------------------------------------------------" << std::endl;
}