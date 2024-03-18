Compile with:
1. On windows: g++ -std=c++20 main.cpp -o myprinter
2. For linux, CMakeLists.txt needs to be updated with proper C++ version (C++20)
3. To run, we need 3 arguments: a string (ABCDEFGHIJ), a character count (3) and a thread count (4) as follows: "./myprinter ABCDEFGHIJ 3 4"
4. To change the number of loops each thread has to run, we have to edit the n_loops variable present in main. The loops are currently not infinite for testing and timing purposes. The number of loops by default is 1,000,000 if this parameter is not passed during initialisation.

Notes:
1. 3 different algorithms are presented as follows (all called with the sample given in the question):
   
	a) sequential.h contains an algorithm that prints substrings sequentially without multithreading.

	b) sema.h contains an algorithm using semaphores (c++20) where the critical section is divided into two parts. This is already faster than mut_cond.h (about 8 times faster, timed with the system clock and 100,000 loops for each thread).
  
	c) max_threading attempts to improve the efficiency even more by utilising all available threads. It uses semaphores and can be easily modified to use fine-grain locks (currently only uses semaphores, c++20). This is even faster than sema.h (about 110 times, using the same 100,000 loops and timed with the system clock)
  
3. The main.cpp file contains the following:
   
	a) the 3 algorithms are initialised
   
	b) Vectors of threads (4) are created, joined and timed one after the other. For each algorithm, the time is calculated before the threads are created and after the threads are joined to maintain synchronisation with the main thread. As per the c++11 memory model which guranatees sequential execution of code, the times thus measured should be accurate.

4. When tested with different loop sizes, the following conclusions were made:

	a) Sequential priniting takes the longest time in all tests.

	b) The time complexity of sema.h is less than O(N). It increases about ~3x if number of loops is increased 10x. This suggests it might be of the order of O(log(N)).

	c) The time complexity of max_threading.h is again about O(N) where N is the number of loops each thread has to run for (10x increase when N is increased by 10x). However, it is still much faster than both the other algorithms for number of loops of the order of ~1,000,000. For loops exceeding 10,000,000 in number, sema.h might be faster since it grows logarithmically in time.

5. The algorithm called max_threading seems to be perfectly starvation-free, as the total time it takes to run only depends on the length of substring that needs to be printed, and it indeed gets faster if more threads are made available. The lower limit to the amount of time required here is set by the std::cout statements only. It is also guaranteed that every substring will be printed n_loops number of times, since we use a blocking call (.aquire()) to acquire the print semaphore.

6. A solution involving max_threading but maintaining a queue of substrings for each thread to print takes much longer than basic max_threading. This is probably because of the push and pop times of the queue adding overheads.
