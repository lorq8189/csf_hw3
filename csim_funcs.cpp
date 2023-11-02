
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <cstring>
#include "csim_funcs.h"

using std::string;
using std::cerr;
using std::strcmp;

bool powerTwo (int x)
{
    if (x < 1)
        return false; //x is negative or 0
    while (x > 1)
    {
        if (x%2 != 0) //x or some factor of it (not including 1) is odd, so false
            return false;
        x /= 2;
    }
    return true; //if it hits 1, its 1 times a power of 2
}

int logTwo(int x) {
    return std::log(x) / std::log(2); //shortcut for log function
}

int readArgument(char** argv, int &num_sets, int &num_blocks, int &num_bytes, 
                            bool &writeAlloc, bool &writeThrough, bool &lru) {
    num_sets = atoi(argv[1]);
    num_blocks = atoi(argv[2]);
    num_bytes = atoi(argv[3]); //loads string arguments into variables
    for (int i=1; i<4; i++)
    {
        if (!powerTwo(atoi(argv[i])))
        {
            cerr << argv[i] << " is not a positive power of 2\n"; //checks if all 3 all powers of two
            return 2;
        }
    }
    if (num_bytes < 4)
    {
        cerr << "number of bytes must be at least 4\n"; //checks for appropriate byte size
        return 2;
    }
    if (strcmp(argv[4], "write-allocate") != 0)     //checks for validity and sets accordingly for write-alloc/not
    {
        if (strcmp(argv[4], "no-write-allocate") == 0)
            writeAlloc = false;
        else    // input error
        {
            cerr << "wrong/no write-allocate input\n";
            return 1;
        }
    }
    return readArgument2(argv, writeAlloc, writeThrough, lru); 
    //passes off intepreting the rest of the args to a helper function
}



int readArgument2(char** argv, bool writeAlloc, bool &writeThrough, bool &lru)
{
    if (strcmp(argv[5],"write-through") != 0)  //checks for validity and sets accordingly for write-through/back
    {
        if (strcmp(argv[5],"write-back") == 0)
        {
            writeThrough = false;
            if (!writeAlloc)
            {
                cerr << "cant combine no-write-allocate and write-back\n"; //these two dont make sense together
                return 2;
            }
        }
        else    // input error
        {
            cerr << "wrong/no write-through/back input\n";
            return 1;
        }
    }
    if (strcmp(argv[6],"lru") != 0)
    {
        if (strcmp(argv[6], "fifo") == 0)
            lru = false;
        else    // input error
        {
            cerr << "wrong/no lru input\n"; //checks for validity and sets accordingly for lru/fifo
            return 1;
        }
    }
    return 0;
}

void initializeCache(Cache &cache, int num_sets, int num_blocks) {

    cache.sets.resize(num_sets);

    for (int setIndex = 0; setIndex < num_sets; setIndex++) {
        Set set;
        set.slots.resize(num_blocks);

        for (Slot& slot : set.slots) { //sets everything to default values at start
            slot.valid = false;
            slot.dirty = false;
            slot.load_ts = 0;
        }

        // Add the set to the cache
        cache.sets[setIndex] = set;
    }
}

int computeSetIndex(uint32_t mem_addr, int set_index_bits, int offset_index_bits) {
    
    // Create a mask to select the bits within the range
    uint32_t mask = ((1U << set_index_bits) - 1) << offset_index_bits;

    // Use the mask to extract the desired bits
    int index = (mem_addr & mask) >> offset_index_bits;

    return index;
    
}

int registerHit(uint32_t mem_addr, int &hits, int &misses, Cache &cache, 
                    int set_index, int tag_index_offset, int &total_cycles, int num_bytes, bool lru, Slot * &target)
{
    Set &set = cache.sets[set_index];
    for (Slot& slot : set.slots) {
        if (slot.valid == true && slot.tag == (mem_addr >> tag_index_offset) ) { //hit case
            hits += 1;
            total_cycles += 1; // load from the cache
            target = &slot;    //on hit, reuse the targetSlot variable for the slot you used
            slot.access_ts = total_cycles;
            return 1;                                   //returns 1 on hit
        }
    }
    //miss case
    misses += 1;
    total_cycles += 100 * (num_bytes / 4); // load from main memory
    // Find an empty slot or the LRU/FIFO slot within the set 
    target = findTarget(set, lru);
    return 0;   //returns 0 on miss
}

Slot * findTarget(Set &set, bool lru)
{
    Slot* targetSlot = &set.slots[0];
    if (lru)
    {
        for (Slot& slot : set.slots) {//iterates through slots, searching for an open one or getting one with earliest access time
            if (!slot.valid) {
                targetSlot = &slot;
                break;
            }
            if (slot.access_ts < (*targetSlot).access_ts) {
                targetSlot = &slot;
            }
        }
    }
    else 
    {
        for (Slot& slot : set.slots) {//same as other case but for fifo, searching for earliest load time instead of access time
            if (!slot.valid) {
                targetSlot = &slot;
                break;
            }
            if (slot.load_ts < (*targetSlot).load_ts) 
                targetSlot = &slot;
        }
    }
    return targetSlot;
}

void loadData(uint32_t mem_addr, int &load_hits, int &load_misses, Cache &cache, 
                    int set_index, int tag_index_offset, int &total_cycles, int num_bytes, bool lru) {

    Slot * targetSlot;
    if(registerHit(mem_addr, load_hits, load_misses, cache, 
                    set_index, tag_index_offset, total_cycles, num_bytes, lru, targetSlot))
        return;  //if registerHit returns 1, you have a hit that you already handled
    //otherwise, proceed with other miss steps

    // Update the selected slot with the new data
    (*targetSlot).tag = mem_addr >> tag_index_offset;
    (*targetSlot).valid = true;
    total_cycles += 1; // store to the cache
    total_cycles += 1; // load from the cache
    (*targetSlot).access_ts = total_cycles; // simulate timestamp update
    (*targetSlot).load_ts = total_cycles;

    return;

}


//v1.1

void storeWriteAlloThru(uint32_t mem_addr, int &store_hits, int &store_misses, 
                            Cache &cache, int set_index, int tag_index_offset, int &total_cycles, int num_bytes, bool lru) {
    // check store hit

     Slot * targetSlot;
    if (registerHit(mem_addr, store_hits, store_misses, cache, 
                    set_index, tag_index_offset, total_cycles, num_bytes, lru, targetSlot))
    {
        total_cycles += 100;
        return;  //if registerHit returns 1, you have a hit, but you need to add 100 cycles after
    }
    //otherwise, proceed with other miss steps
    // Update the selected slot with the new data
    (*targetSlot).tag = mem_addr >> tag_index_offset;
    (*targetSlot).valid = true;
    total_cycles += 1 ; // store to the cache
    total_cycles += 100; // simulate write_though: store immediately to the main memory
    (*targetSlot).access_ts = total_cycles; // Simulate timestamp update
    (*targetSlot).load_ts = total_cycles;
    return;

}

void storeWriteAlloBack(uint32_t mem_addr, int &store_hits, int &store_misses, 
                            Cache &cache, int set_index, int tag_index_offset, int &total_cycles, int num_bytes, bool lru) {
    
    // check store hit
    Slot * targetSlot;
    if (registerHit(mem_addr, store_hits, store_misses, cache, 
                    set_index, tag_index_offset, total_cycles, num_bytes, lru, targetSlot))
    {
        targetSlot->dirty = true;
        return;  //if registerHit returns 1, you have a hit, but you need to mark that slot as dirty
    }
    //otherwise, continue with rest of steps on a miss
    // simulate write back: eviction of data -> write back to main memory if it's marked dirty
    if ((*targetSlot).valid && (*targetSlot).dirty) {
        total_cycles += 100 * (num_bytes / 4); // simulate write-back: write back to main memory when data get evicted 
    }

    // Update the selected slot with the new data
    (*targetSlot).tag = mem_addr >> tag_index_offset;
    (*targetSlot).valid = true;
    total_cycles += 1; // store to the cache
    (*targetSlot).dirty = true; // simulate write-back: modify the block stored in cache
    (*targetSlot).access_ts = total_cycles; // Simulate timestamp update
    (*targetSlot).load_ts = total_cycles;

    return;

}

// i would use the helper function for this one but its already short enough
void storeWriteNoAlloThru(uint32_t mem_addr, int &store_hits, int &store_misses, 
                            Cache &cache, int set_index, int tag_index_offset, int &total_cycles, int num_bytes) {
    
    // check store hit
    Set &set = cache.sets[set_index];

    for (Slot& slot : set.slots) {
        if (slot.valid == true && slot.tag == (mem_addr >> tag_index_offset) ) {
            store_hits += 1;
            total_cycles += 1; // store to the cache
            total_cycles += 100; // simulate write_though: store immediately to the main memory
            slot.access_ts = total_cycles;
            return;
        }
    }

    store_misses += 1;
    total_cycles += 100; // simulate write-through: store immediately to the main memory

    return; 

}


// void loadDataFull(uint32_t mem_addr, int &load_hits, int &load_misses, Cache &cache, 
//                     int tag_index_offset, int &total_cycles, int num_bytes) {

//     Set &set = cache.sets[0];
// }
