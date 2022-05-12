#ifndef BTB_H
#define BTB_H

#include <inttypes.h>
#include <stdlib.h>

class BranchTargetBuffer
{
public:
    int32_t size;
    int mode;
    int32_t *targets;
    int32_t *predictionBits;

    // prediction bits:
    // 0 strongly dont take
    // 1 weakly dont take
    // 2 weakly take
    // 3 strongly take

    BranchTargetBuffer(int32_t _size, int _mode)
    {
        mode = _mode;
        int default_val;
        switch (mode)
        {
        case 0: // Two bit
            default_val = 2;
            break;
        case 1: // Always take
            default_val = 3;
            break;
        case 2: // Always dont take
            default_val = 0;
            break;
        default:
            default_val = 2;
            break;
        }

        size = _size;
        targets = (int32_t *)calloc(size, sizeof(int32_t));
        predictionBits = (int32_t *)calloc(size, sizeof(int32_t));
        for (int i = 0; i < size; i++)
        {
            targets[i] = -1;                 // -1 to signify empty
            predictionBits[i] = default_val; // default to weakly take
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