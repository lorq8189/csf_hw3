# csf_hw3
# Team Members: Joanna Cheng, Lucas Orquiza

Joanna did most of everything needed by MS2, (so most of the actual bulk code was done by her) all cache types for LRU, 
the struct setup we used, and the main function.

Lucas did input processing/error handling, FIFO cases, modularized the code into helper functions, wrote a few 
of the helper functions, and everything in the README including the testing for best cache.

both of us were responsible for commenting our individual parts

factors: 
number of sets
number of blocks
how many bytes per block
write-alloc/not
write back/through
LRU/FIFO





Commands and stuff to copy:

rm -f solution.zip
zip -9r solution.zip Makefile *.h *.cpp README.txt

./csim 256 4 16 write-allocate write-back lru < gcc.trace