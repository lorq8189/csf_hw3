#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include "csim_funcs.h"

using std::cin; using std::cout;
using std::ofstream;
using std::ifstream;
using std::string;
using std::cerr;
using std::getline;


/*number of sets in the cache (a positive power-of-2)
number of blocks in each set (a positive power-of-2)
number of bytes in each block (a positive power-of-2, at least 4)
write-allocate or no-write-allocate
write-through or write-back
lru (least-recently-used) or fifo evictions*/


int main(int argc, char** argv ) {


    int sets, blocks, bytes, exptest;
    sets = atoi(argv[1]);
    blocks = atoi(argv[2]);
    bytes = atoi(argv[3]);

    for (int i=1; i<4; i++)
    {
        if (!powerTwo(atoi(argv[i])))
        {
            cerr << argv[i] << " is not a positive power of 2\n";
            return 2;
        }
    }
    if (bytes < 4)
    {
        cerr << "number of bytes must be at least 4\n";
        return 2;
    }

    bool writeAlloc = true;
    if (argv[4] != "write-allocate")
    {
        if (argv[4] == "no-write-allocate")
            writeAlloc = false;
        else    // input error
        {
            cerr << "wrong/no write-allocate input\n";
            return 1;
        }
    }

    bool writeThrough = true;
    if (argv[5] != "write-through")
    {
        if (argv[5] == "write-back")
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

    bool lru = true;
    if (argv[6] != "lru")
    {
        if (argv[6] == "fifo")
            lru = false;
        else    // input error
        {
            cerr << "wrong/no lru input\n";
            return 1;
        }
    }

// Total loads: count       data[0]
// Total stores: count
// Load hits: count
// Load misses: count
// Store hits: count
// Store misses: count
// Total cycles: count      data[6]

    int data[7] = {0, 0, 0, 0, 0, 0, 0}; //list of stats to keep, in the order above.

    string line = "";
    char load = '\0';
    int eol = 0;
    
    while (getline(cin, line))
    {
        //first parse through to get the mode
        if (line[0]== 'l')
            ;
        else { //load

        } //save

        //read address here

        eol = line[13];
        //input data here
    }

    




    return 0;
}
