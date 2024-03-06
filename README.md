Compile with:
1. On windows: g++ -std=c++20 main.cpp -o myprinter
2. For linux, CMakeLists.txt needs to be updated with proper C++ version (C++20)
3. To run, we need 3 arguments: a string (ABCDEFGHIJ), a character count (3) and a thread count (4) as follows: "./myprinter ABCDEFGHIJ 3 4"
4. To change the number of loops each thread has to run, we have to edit the n_loops variable present in main. The loops are currently not infinite for testing and timing purposes. The number of loops by default is 1,000,000 if this parameter is not passed during initialisation.

Notes:
1. 3 different algorithms are presented as follows (all called with the sample given in the question):
  a) mut_cond.h contains an algorithm using mutexes and condition_variables. These are c++11 features. The algorithm ends up running sequantially for the most part.

  b) sema.h contains an algorithm using semaphores (c++20) where the critical section is divided into two parts. This is already much faster than mut_cond.h (about 80 times faster, times with system clock and 100,000 loops for each thread).
  
  c) max_threading attempts to improve the efficiency even more by utilising all availabl;e threads. It uses semaphores and can be easily modified to use fine-grain locks (currently only uses semaphores, c++20). This is even faster than sema.h (about 110 times, using the same 100,000 loops and timed with system clock)
  
3. The main.cpp file contains the following:
   a) the 3 algorithms are initialised
   
   b) vectors of threads (4) are created, joined and timed one after the other. For each algorithm, the time is calculated before the threads are created and after the threads are joined to maintain synchronisation with the main thread. As per the c++11 memory model which guranatees sequential execution of code, the times thus measured should be accurate.
