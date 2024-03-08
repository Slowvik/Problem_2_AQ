/*
> Notes on the algorithm:
    > Use every available thread to calculate and keep a substring ready to print
    > Only one thread can acquire the print_sema at a time, since printing needs to be sequential
    > Once a thread is done printing, it calculates its next starting position and keeps it ready
    > Should work similarly to a parallel for_each, except it's more like for_each_block that is sequential
    > Can possibly maintain a queue of starting pos for every thread, so that when it gets to print, it can pop the front and print.
    > Cannot really use a vector of semaphores, but it was possible to use a vector of mutexes (fine grain locks). However, I wanted to use only semaphores as they are a more lightweight version of mutexes (semaphores are c++20, mutexes are c++11) and did not want to mix mutexes with semaphores.
*/
#ifndef MAX_THREADING_H_
#define MAX_THREADING_H_
#include <iostream>
#include <semaphore>
#include <thread>
#include <chrono>
#include <vector>

namespace max_threading
{
    int num_loops = 0;
    std::string main_string;
    int string_size;
    int char_count;
    int thread_count;

    int start_index;//start_index will be the first char of new substring, 1+ last char of previous substring
    std::vector<int> next_start_index;

    //std::vector<std::binary_semaphore> index_update_sema;//Cannot do this, instead:
    std::vector<bool> index_is_updated;

    std::vector<bool> print_allowed;

    void init(std::string s, int c_count, int t_count, int n=0)
    {
        max_threading::num_loops = n;
        max_threading::main_string = s;
        max_threading::string_size = s.size();
        max_threading::char_count = c_count;
        max_threading::thread_count = t_count;

        for(int i=0;i<max_threading::thread_count;i++)
        {
            std::binary_semaphore b(1);
            // max_threading::index_update_sema.push_back(b);
            // max_threading::index_update_sema[i].acquire();
            max_threading::index_is_updated.push_back(true);
            max_threading::next_start_index.push_back((i*max_threading::char_count)%max_threading::string_size);
        }

        max_threading::print_allowed.push_back(true);
        for(int i=1; i<max_threading::thread_count;i++)
        {
            max_threading::print_allowed.push_back(false);
        }
    }    

    std::binary_semaphore print_sema(1);
    void critical_print(int thread_ID)
    {
        std::string subs;
        int count = 0;
        int pos = max_threading::next_start_index[thread_ID];
        while (count<char_count)
        {
            subs+=max_threading::main_string[pos%max_threading::string_size];
            pos++;
            count++; 
        }

        std::cout<<subs<<std::endl;
    }

    void index_update(int thread_ID)
    {
        if(!max_threading::index_is_updated[thread_ID])
        {            
            max_threading::next_start_index[thread_ID] = (max_threading::next_start_index[thread_ID]+(max_threading::thread_count*max_threading::char_count))%max_threading::string_size;
            //std::cout<<"Updating index for thread "<<thread_ID<<" to "<<max_threading::next_start_index[index]<<std::endl;
            max_threading::index_is_updated[thread_ID] = true;
        }        
    }

    void thread_runner(int thread_ID)
    {
        int count=0;
        while(count++<num_loops)
        {
            //Tries to update index and prepare to extract next substring assigned to it:
            max_threading::index_update(thread_ID);

            if(max_threading::print_allowed[thread_ID] && max_threading::index_is_updated[thread_ID])
            {
                //std::cout<<"Print allowed for thread "<<index<<std::endl;
                max_threading::print_sema.acquire();
                max_threading::critical_print(thread_ID);
                max_threading::print_allowed[thread_ID]=false;
                max_threading::print_allowed[(thread_ID+1)%max_threading::thread_count]=true;//for sequential printing
                
                //max_threading::index_update_sema[index].release();
                max_threading::index_is_updated[thread_ID] = false;
                max_threading::print_sema.release();
            }
        }
    }
}

#endif