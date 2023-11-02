# csf_hw3
# Team Members: Joanna Cheng, Lucas Orquiza

Joanna did most of everything needed by MS2, (so most of the actual bulk code was done by her) all cache types for LRU, 
the struct setup we used, and the main function.

Lucas did input processing/error handling, FIFO cases, modularized the code into helper functions, wrote a few 
of the helper functions, and everything in the README including the testing for best cache.

both of us were responsible for commenting our individual parts



Report on best cache:

To start, In testing I did not use any of the small test files, mainly gcc.trace and swim.trace to validate results.
The reason behind this is that the asymptotic speed is more important where outside factors wont cause as much of a disruption,
the larger files are more consistent, and are more accurate to what a CPU actually processes as opposed to tiny amounts of code.
In terms of the caches we are allowed to simulate, there are only 6 factors that vary:

number of sets
number of blocks
how many bytes per block
write-alloc/not
write back/through
LRU/FIFO

Our testing was checking the optimal setting for each of these variables individually, with a few exceptions, and then
testing the combination of the optimal settings (a local max ) with other combinations 

First, we grouped the number of sets, blocks, and bytes per block into their own testing phase. The point of this is to determine
the optimal relative levels between the three (many sets with few blocks vs few sets with all the blocks vs moderate amount of sets
and blocks but with many bytes per block etc.) before moving on to total data stored in the cache
The conclusion from the testing was that the ordering of blocks > sets > bytes/block usually had the least amount of cycles
with the same amount of overhead, even if they did have more misses in general, the speed of the system made up for the time lost
by misses. Our final division was 32:128:4 for a 16kb cache

Then, we tested them by writing modes. out of write-alloc/no-alloc and write back/through, there are only 3 relevant combinations,
and we tested them all with our original configuration that minimized total cycles, and two others that minimized misses to 
different degrees. For all of these, the combination of write-alloc and write-back outpaced the other two by 2-4 times, and had an equal or higher
hit rate than the other two. 

Then, we ordered them by total cache size, with the same/similar ratio from before. The tests first showed that instead of multiplying
everything by the same amount, it was a better wast of space to only increase the number of sets and blocks. Doing that revealed
(for trace file with the size of gcc.trace) that there were diminishing or no returns past 256 kB total size. testing on swim.trace
revealed the same but for 1 MB. The conclusion we got from this is that with out set/block configuration, the larger the cache 
the better up until a certain point, at which its not at all useful.
Now in terms of caches it wasnt specified which level of cache we were modeling, but that amount of 
storage space only makes sense for LLC. For L1 cache, something like the default size from the course website example makes sense,
and for L2, something like 128, 256, 4 works best. It really depends on how much space is practical, and how close to the processor
the cache is.

Lastly is LRU/FIFO, which wasnt very hard to figure out. the difference doesnt really matter until you need to evict, 
meaning it doesnt have an effect on hit rate or much else. LRU was pretty much always faster in terms of total cycles by a
significant margin, in the tests. The tests themselves were just testing the two with some of the tests we did before.

Our conclusions for the best cache are as follows: a cache with write-alloc, write back, lru, 4 bytes per block, and sets to blocks
in a ratio of 1:4 with the amount of either depending on the type of cache and the amount of space allowed. We tested this by taking
up various tests that we tried before, mixing them up, and comparing them to our current best,
 but changing 1-2 factors and running them all on swim.trace, and we got no new best/significant results.







Commands and stuff to copy:

rm -f solution.zip
zip -9r solution.zip Makefile *.h *.cpp README.txt

./csim 256 4 16 write-allocate write-back lru < gcc.trace