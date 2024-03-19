/*
> Note on the algorithm:
    > The critical section here is divided into two parts: 
    > In the first part (bound by _print_sema), we extract and print the substring
    > In the second part (bound by _calc_sema), we calculate the next starting position for the next substring and push it onto a queue.
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
    int _num_loops = 0;
    std::string _main_string;
    int _string_size; 
    int _char_count;
    int _thread_count;
        
    int _start_index;//_start_index will be the first char of new substring, 1+ last char of previous substring   
    std::queue<int> _start_index_queue;    

    std::binary_semaphore _print_sema(1);
    std::binary_semaphore _calc_sema(1);

    void init(std::string s, int c_count, int t_count, int n=0)
    {
        _num_loops = n;
        _main_string = s;
        _string_size = s.size();
        _char_count = c_count;
        _thread_count = t_count;
        _start_index_queue.push(0);
    }
    
    //Extracts a substring based on user-given parameters
    std::string returnSubstring(int index)
    {
        std::string subs;
        int count = 0;
        int pos = index;
        while (count<_char_count)
        {
            subs+=_main_string[pos%_string_size];
            pos++;
            count++; 
        }
        return subs;
    }

    //Calculates starting position of next substring
    void calcNextStartIndex()
    {
        _start_index = (_start_index+_char_count)%_string_size;
        _start_index_queue.push(_start_index);
    }

    void threadRunner(int index)
    {
        int count=0;
        while(count++<_num_loops)
        {
            //CRITICAL SECTION 1
            /*            
            > Each thread moves on without blocking if it can't aquire the _print_sema. Only one thread can acquire _print_sema because this section contains both read(calculate substring) and write(pop from _start_index_queue): operations which need to happen together sequentially.
            */
            if(_print_sema.try_acquire())
            {                
                if(!_start_index_queue.empty())
                {
                    std::cout<<returnSubstring(_start_index_queue.front())<<std::endl;
                    _start_index_queue.pop();                    
                    //std::this_thread::sleep_for(std::chrono::milliseconds(1));
                }

                _print_sema.release();
            }
           
            //CRITICAL SECTION 2
            /*
            > Same concept, we don't want to block a thread till it acquires the _calc_sema, so we use try_aquire().
            > This is a again a read-write operation, we read the value of _start_index and write(push) a value into _start_index_queue, so only one thread can access this section at a time.
            */
            if(_calc_sema.try_acquire())
            { 
                calcNextStartIndex();
                _calc_sema.release();
            }            
        }        
    }
}

#endif