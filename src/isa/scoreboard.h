#ifndef SCOREBOARD_H
#define SCOREBOARD_H

#include "isa.h"
class Scoreboard
{
public:
    virtual bool isValid(REGISTER_ABI_NAME reg);

    virtual void setValid(REGISTER_ABI_NAME reg) {}

    virtual void setInvalid(REGISTER_ABI_NAME reg) {}

    virtual void log();
};

class SimpleScoreboard : public Scoreboard
{
    bool valid[REGABI_UNUSED];

public:
    // Simple map from a register ABI name to a valid bit

    SimpleScoreboard();

    bool isValid(REGISTER_ABI_NAME reg);

    void setValid(REGISTER_ABI_NAME reg);

    void setInvalid(REGISTER_ABI_NAME reg);

    void log();
};

#endif