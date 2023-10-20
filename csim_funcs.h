#ifndef CSIM_FUNCS_H
#define CSIM_FUNCS_H

#include <cstdint>
#include <vector>


struct Slot {
    uint32_t tag;
    bool valid;
    uint32_t load_ts,
    access_ts;
};
struct Set {
    std::vector<Slot> slots;
};
struct Cache {
    std::vector<Set> sets;
};

bool powerTwo (int x);



#endif