#ifndef MAIN_H
#define MAIN_H

#include <inttypes.h>

enum PROCESSOR_MODES
{
    SCALAR,
    TOMASULOS,
    INVALID
};

enum BRANCH_PREDICT_MODE
{
    TWO_BIT,
    ALWAYS_TAKE,
    NEVER_TAKE
};

int mode = INVALID;
int bp_mode = TWO_BIT;

#endif