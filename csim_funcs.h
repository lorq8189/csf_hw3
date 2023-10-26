#ifndef CSIM_FUNCS_H
#define CSIM_FUNCS_H

#include <cstdint>
#include <vector>
#include <string>


struct Slot {
    uint32_t tag;
    bool valid;
    uint32_t load_ts,
    access_ts;          
    bool dirty;
};
struct Set {
    std::vector<Slot> slots;
};
struct Cache {
    std::vector<Set> sets;
};


// Return whether the number is a power of two
bool powerTwo (int x);

// Compute logX
int logTwo(int x);

// Read arguments from command line
int readArgument(char** argv, int &num_sets, int &num_blocks, int &num_bytes, bool &writeAlloc, bool &writeThrough, bool &lru);

// initialize Cache data structure
void initializeCache(Cache &cache, int num_sets, int num_blocks);

// Compute set index
int computeSetIndex(uint32_t mem_addr, int set_index_bits, int offset_index_bits);

// Load data configured with m-way set-associative
void loadData(uint32_t mem_addr, int &load_hits, int &load_misses, Cache &cache, 
                    int set_index, int tag_index_offset, int &total_cycles, int num_bytes, bool lru);

// Store data configured with m-way set-associative, write-allocate, write-through
void storeWriteAlloThru(uint32_t mem_addr, int &store_hits, int &store_misses, 
                            Cache &cache, int set_index, int tag_index_offset, int &total_cycles, int num_bytes, bool lru);

// Store data configured with m-way set-associative, write-allocate, write-back
void storeWriteAlloBack(uint32_t mem_addr, int &store_hits, int &store_misses, 
                            Cache &cache, int set_index, int tag_index_offset, int &total_cycles, int num_bytes, bool lru);

// Store data configured with m-way set-associative, no-write-allocate, write-through
void storeWriteNoAlloThru(uint32_t mem_addr, int &store_hits, int &store_misses, 
                            Cache &cache, int set_index, int tag_index_offset, int &total_cycles, int num_bytes);


void loadDataFull(uint32_t mem_addr, int &load_hits, int &load_misses, Cache &cache, 
                    int tag_index_offset, int &total_cycles, int num_bytes);


#endif