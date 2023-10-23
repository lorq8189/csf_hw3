#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <sstream>
#include "csim_funcs.h"

using std::cin; using std::cout;
using std::ofstream;
using std::ifstream;
using std::string;
using std::cerr;
using std::strcmp;


int main(int argc, char** argv ) {


    // read arguments from command line
    int num_sets, num_blocks, num_bytes;
    bool writeAlloc = true;
    bool writeThrough = true; 
    bool lru = true;

    int res = readArgument(argv, num_sets, num_blocks, num_bytes, writeAlloc, writeThrough, lru);
    if (res != 0) {
        return res;
    }

    // initialize cache configuration
    Cache cache;
    initializeCache(cache, num_sets, num_blocks);

    int offset_index_bits = logTwo(num_bytes);
    int set_index_bits = logTwo(num_sets);
    int tag_index_offset = offset_index_bits + set_index_bits;

    // initialize counting variables
    int total_loads = 0;
    int total_stores = 0;
    int load_hits = 0;
    int load_misses = 0;
    int store_hits = 0;
    int store_misses = 0;
    int total_cycles = 0;

    // read from memory trace
    string line;
    while (std::getline(cin, line)) {
        std::istringstream iss(line);
        char operation = ' ';
        uint32_t mem_addr;
        int size;

        // read the components separated by spaces
        iss >> operation;
        iss >> std::hex >> mem_addr;
        iss >> size;

        // get set index
        int set_index = computeSetIndex(mem_addr, set_index_bits, offset_index_bits);
        
        if (operation == 'l') { // load data
            total_loads += 1;

            if (num_sets > 1) { // m-way set-associative
                loadDataMWay(mem_addr, load_hits, load_misses, cache, set_index, 
                                tag_index_offset, total_cycles, num_bytes);
            } else if (num_sets == 1) {
                loadDataMWay(mem_addr, load_hits, load_misses, cache, 0, 
                                tag_index_offset, total_cycles, num_bytes);
            }

        } else { // store data

            total_stores += 1;
            if (num_sets > 1) { // m-way set-associative
                // write_allocate & write_through
                if (writeAlloc == true && writeThrough == true) {
                    storeWriteAlloThruMway(mem_addr, store_hits, store_misses, 
                            cache, set_index, tag_index_offset, total_cycles, num_bytes);
                } else if (writeAlloc == true && writeThrough == false) { // write_allocate & write back
                    storeWriteAlloBackMway(mem_addr, store_hits, store_misses, 
                            cache, set_index, tag_index_offset, total_cycles, num_bytes);
                } else { // no write_allo & write_through
                    storeWriteNoAlloThruMway(mem_addr, store_hits, store_misses, 
                            cache, set_index, tag_index_offset, total_cycles, num_bytes);
                }
            } else if (num_sets == 1) {

                // write_allocate & write_through
                if (writeAlloc == true && writeThrough == true) {
                    storeWriteAlloThruMway(mem_addr, store_hits, store_misses, 
                            cache, 0, tag_index_offset, total_cycles, num_bytes);
                } else if (writeAlloc == true && writeThrough == false) { // write_allocate & write back
                    storeWriteAlloBackMway(mem_addr, store_hits, store_misses, 
                            cache, 0, tag_index_offset, total_cycles, num_bytes);
                } else { // no write_allo & write_through
                    storeWriteNoAlloThruMway(mem_addr, store_hits, store_misses, 
                            cache, 0, tag_index_offset, total_cycles, num_bytes);
                }
            }

        }
        
    }

    // output counting statistics
    cout << "Total loads: " << total_loads << std::endl;
    cout << "Total stores: " << total_stores << std::endl;
    cout << "Load hits: " << load_hits << std::endl;
    cout << "Load misses: " << load_misses << std::endl;
    cout << "Store hits: " << store_hits << std::endl;
    cout << "Store misses: " << store_misses << std::endl;
    cout << "Total cycles: " << total_cycles << std::endl;

    return 0;
}
