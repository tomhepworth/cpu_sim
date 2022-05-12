#ifndef BTB_H
#define BTB_H

#include <inttypes.h>
#include <stdlib.h>

class BranchTargetBuffer
{
public:
    int32_t size;
    int32_t *targets;
    int32_t *predictionBits;

    // 0 strongly dont take
    // 1 weakly dont take
    // 2 weakly take
    // 3 strongly take

    BranchTargetBuffer(int32_t _size)
    {
        size = _size;
        targets = (int32_t *)calloc(size, sizeof(int32_t));
        predictionBits = (int32_t *)calloc(size, sizeof(int32_t));
        for (int i = 0; i < size; i++)
        {
            targets[i] = -1;       // -1 to signify empty
            predictionBits[i] = 2; // default to weakly take
        }
    }

    void print()
    {
        for (int i = 0; i < size; i++)
        {
            if (targets[i] != -1)
            {
                std::cout << i << "\t" << targets[i] << "\t" << predictionBits[i] << std::endl;
            }
        }
    }
};

#endif