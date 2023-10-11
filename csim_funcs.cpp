#include <iostream>
#include <fstream>
#include <string>
#include "csim_funcs.h"

bool powerTwo (int x)
{
    if (x < 1)
        return false;
    while (x > 1)
    {
        if (x%2 != 0)
            return false;
        x /= 2;
    }
    return true;
}


