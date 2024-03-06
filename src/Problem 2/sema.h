/*
> Note on the algorithm:
    > The critical section here is divided into two parts: 
    > In the first part (bound by print_sema), we extract and print the substring
    > In the second part (bound by calc_sema), we calculate the next starting position for the next substring and push it onto a queue.
*/
#ifndef SEMA_H_
#define SEMA_H_
#include <iostream>
#include <thread>
#include <semaphore>
#include <vector>
#include <windows.h>
#include <queue>
#include <chrono>

namespace sema
{
    int num_loops = 0;
    std::string main_string;
    int string_size; 
    int char_count;
    int thread_count;
        
    int start_index;//start_index will be the first char of new substring, 1+ last char of previous substring   
    std::queue<int> start_index_queue;    

    std::binary_semaphore print_sema(1);
    std::binary_semaphore calc_sema(1);

    void init(std::string s, int c_count, int t_count, int n=0)
    {
        sema::num_loops = n;
        sema::main_string = s;
        sema::string_size = s.size();
        sema::char_count = c_count;
        sema::thread_count = t_count;
        sema::start_index_queue.push(0);
    }
    
    //Extracts a substring based on user-given parameters
    std::string return_substring(int index)
    {
        std::string subs;
        int count = 0;
        int pos = index;
        while (count<char_count)
        {
            subs+=sema::main_string[pos%sema::string_size];
            pos++;
            count++; 
        }
        return subs;
    }

    //Calculates starting position of next substring
    void calc_next_start_index()
    {
        sema::start_index = (sema::start_index+sema::char_count)%sema::string_size;
        sema::start_index_queue.push(sema::start_index);
    }

    void thread_runner(int index)
    {
        int count=0;
        while(count++<num_loops)
        {
            //CRITICAL SECTION 1
            /*            
            > Each thread moves on without blocking if it can't aquire the print_sema. Only one thread can acquire print_sema because this section contains both read(calculate substring) and write(pop from start_index_queue): operations which need to happen together sequentially.
            */
            if(sema::print_sema.try_acquire())
            {                
                //std::cout<<"In print section with thread: "<<index<<std::endl;
                if(!sema::start_index_queue.empty())
                {
                    std::cout<<sema::return_substring(sema::start_index_queue.front())<<std::endl;
                    sema::start_index_queue.pop();                    
                    //std::this_thread::sleep_for(std::chrono::milliseconds(1));
                }

                sema::print_sema.release();
            }
           
            //CRITICAL SECTION 2
            /*
            > Same concept, we don't want to block a thread till it acquires the calc_sema, so we use try_aquire().
            > This is a again a read-write operation, we read the value of start_index and write(push) a value into start_index_queue, so only one thread can access this section at a time.
            */
            if(sema::calc_sema.try_acquire())
            {
                //std::cout<<"In calc section with thread: "<<index<<std::endl;  
                sema::calc_next_start_index();

                sema::calc_sema.release();
            }            
        }        
    }
}

#endif