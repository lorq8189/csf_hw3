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


/*number of sets in the cache (a positive power-of-2)
number of blocks in each set (a positive power-of-2)
number of bytes in each block (a positive power-of-2, at least 4)
write-allocate or no-write-allocate
write-through or write-back
lru (least-recently-used) or fifo evictions*/


int main(int argc, char** argv ) {


    // read arguments from command line
    int sets, blocks, bytes, exptest;
    sets = atoi(argv[1]);
    blocks = atoi(argv[2]);
    bytes = atoi(argv[3]);

    bool writeAlloc = true;
    bool writeThrough = true;
    bool lru = true;

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

    // read from memory trace
    string line;
    while (std::getline(cin, line)) {
        std::istringstream iss(line);
        string operation;
        string mem_addr;
        int size;

        // Read the components separated by spaces
        iss >> operation >> mem_addr >> size;

        
        break;
    }

    return 0;
}
