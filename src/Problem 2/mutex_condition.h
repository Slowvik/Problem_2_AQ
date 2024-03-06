/*
Algorithm:
    > Two loops: external over number of threads, and each thread has an infinite loop inside
    > Inside the infinite loop: 
        > try to acquire the mutex
        > wait, release mutex atomically and wait for the condition (your turn)
        > if condition met, calculate and print
        > release the lock
        > notify all other threads

Possible modification if the calculations are time-consuming:
    > If the order in which threads are called can be random, each thread can be asked to calculate and keep the next substring ready while other threads are printing
    > If two or more threads have calculated substrings ready, some sort of check needs to be in place to see ho prints first
    > Printing will still be sequential (using the same condition variable), but the calculation of substring can possibly be done in advance by a random thread
*/
#ifndef MUTEX_COND_H_
#define MUTEX_COND_H_

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <string>
#include <vector>
#include <atomic>

namespace mut_cond
{
    std::mutex main_mutex;
    std::condition_variable main_cond;

    int thread_count;
    std::vector<bool> turn;    
    std::atomic<int> start_index=0;//start_index will be the first char of new substring, 1+ last char of previous substring

    int string_size;
    std::string main_string;
    int char_count;

    //Extracts a substring sequentially based on user-given parameters
    std::string return_substring()
    {
        std::string subs;
        int count = 0;
        int pos = mut_cond::start_index;
        while (count<char_count)
        {
            subs+=mut_cond::main_string[pos%mut_cond::string_size];
            pos++;
            count++; 
        }
        return subs;
    }

    //Calculates starting position of next substring
    void calc_next_start_index()
    {
        mut_cond::start_index = (mut_cond::start_index+mut_cond::char_count)%mut_cond::string_size;
    }


    //thread_runner currently prints the substrings in an infinite loop
    void thread_runner(int index)
    {
        while(1)
        {
            //ENTRY SECTION

            //acquire the mutex
            std::unique_lock<std::mutex> thread_lock(main_mutex);

            //condition_variable atomically releases the mutex till predicate is met (till it's turn comes)
            main_cond.wait(thread_lock, [&]{return turn[index];});

            //CRITICAL SECTION

            //Printing the substring
            std::cout<<mut_cond::return_substring()<<std::endl; 

            //calculate start index for next thread
            mut_cond::calc_next_start_index();           

            //Change own turn[index] to false;
            turn[index] = false;

            //Change next thread's turn[(index+1)%thread_count] to true - results in threads being called sequentially.
            turn[(index+1)%mut_cond::thread_count] = true;

            //EXIT SECTION

            //release the mutex
            thread_lock.unlock();

            //notify waiting threads
            main_cond.notify_all();
        }
    }
}

#endif