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
        return false;
    while (x > 1)
    {
        if (x%2 != 0)
            return false;
        x /= 2;
    }
    return true;
}

int logTwo(int x) {
    return std::log(x) / std::log(2);
}

int readArgument(char** argv, int &num_sets, int &num_blocks, int &num_bytes, 
                            bool &writeAlloc, bool &writeThrough, bool &lru) {

    num_sets = atoi(argv[1]);
    num_blocks = atoi(argv[2]);
    num_bytes = atoi(argv[3]);

    for (int i=1; i<4; i++)
    {
        if (!powerTwo(atoi(argv[i])))
        {
            cerr << argv[i] << " is not a positive power of 2\n";
            return 2;
        }
    }
    if (num_bytes < 4)
    {
        cerr << "number of bytes must be at least 4\n";
        return 2;
    }

    if (strcmp(argv[4], "write-allocate") != 0)
    {
        if (strcmp(argv[4], "no-write-allocate") == 0)
            writeAlloc = false;
        else    // input error
        {
            cerr << "wrong/no write-allocate input\n";
            return 1;
        }
    }

    if (strcmp(argv[5],"write-through") != 0)
    {
        if (strcmp(argv[5],"write-back") == 0)
        {
            writeThrough = false;
            if (!writeAlloc)
            {
                cerr << "cant combine no-write-allocate and write-back\n";
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
            cerr << "wrong/no lru input\n";
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

        for (Slot& slot : set.slots) {
            slot.valid = false;
            slot.dirty = false;
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

void loadData(uint32_t mem_addr, int &load_hits, int &load_misses, Cache &cache, 
                    int set_index, int tag_index_offset, int &total_cycles, int num_bytes) {

    Set &set = cache.sets[set_index];

    for (Slot& slot : set.slots) {
        if (slot.valid == true && slot.tag == (mem_addr >> tag_index_offset) ) {
            load_hits += 1;
            total_cycles += 1; // load from the cache
            slot.access_ts = total_cycles;
            return;
        }
    }

    load_misses += 1;
    total_cycles += 100 * (num_bytes / 4); // load from main memory
    // Find an empty slot or the LRU slot within the set
    Slot* targetSlot = &set.slots[0];
    for (Slot& slot : set.slots) {
        if (!slot.valid) {
            targetSlot = &slot;
            break;
        }
        if (slot.access_ts < (*targetSlot).access_ts) {
            targetSlot = &slot;
        }
    }

    // Update the selected slot with the new data
    (*targetSlot).tag = mem_addr >> tag_index_offset;
    (*targetSlot).valid = true;
    total_cycles += 1; // store to the cache
    total_cycles += 1; // load from the cache
    (*targetSlot).access_ts = total_cycles; // simulate timestamp update

    return;

}

void storeWriteAlloThru(uint32_t mem_addr, int &store_hits, int &store_misses, 
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
    total_cycles += 100 * (num_bytes / 4); // simulate write_allocate: bring memory block from main memory to cache
    // Find an empty slot or the LRU slot within the set
    Slot* targetSlot = &set.slots[0];
    for (Slot& slot : set.slots) {
        if (!slot.valid) {
            targetSlot = &slot;
            break;
        }
        if (slot.access_ts < (*targetSlot).access_ts) {
            targetSlot = &slot;
        }
    }

    // Update the selected slot with the new data
    (*targetSlot).tag = mem_addr >> tag_index_offset;
    (*targetSlot).valid = true;
    total_cycles += 1 ; // store to the cache
    total_cycles += 100; // simulate write_though: store immediately to the main memory
    (*targetSlot).access_ts = total_cycles; // Simulate timestamp update

    return;

}

void storeWriteAlloBack(uint32_t mem_addr, int &store_hits, int &store_misses, 
                            Cache &cache, int set_index, int tag_index_offset, int &total_cycles, int num_bytes) {
    
    // check store hit
    Set &set = cache.sets[set_index];

    for (Slot& slot : set.slots) {
        if (slot.valid == true && slot.tag == (mem_addr >> tag_index_offset) ) { // store hit
            store_hits += 1;
            total_cycles += 1; // store to the cache
            slot.dirty = true; // simulate write-back: modify the block in cache and mark it dirty
            slot.access_ts = total_cycles;
            return;
        }
    }

    store_misses += 1;
    total_cycles += 100 * (num_bytes / 4); // simulate write_allocate: bring memory block from main memory to cache
    // Find an empty slot or the LRU slot within the set
    Slot* targetSlot = &set.slots[0];
    for (Slot& slot : set.slots) {
        if (!slot.valid) {
            targetSlot = &slot;
            break;
        }
        if (slot.access_ts < (*targetSlot).access_ts) {
            targetSlot = &slot;
        }
    }

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

    return;

}


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
    total_cycles += 100; // simulate write-through: store immediatly to the main memory

    return;

}


