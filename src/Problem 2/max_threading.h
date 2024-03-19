/*
> Notes on the algorithm:
    > Use every available thread to calculate and keep a substring ready to print
    > Only one thread can acquire the _print_sema at a time, since printing needs to be sequential
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
    int _num_loops = 0;
    std::string _main_string;
    int _string_size;
    int _char_count;
    int _thread_count;

    int start_index;//start_index will be the first char of new substring, 1+ last char of previous substring
    std::vector<int> _next_start_index;

    //std::vector<std::binary_semaphore> indexUpdate_sema;//Cannot do this, instead:
    std::vector<bool> _index_is_updated;

    std::vector<bool> _print_allowed;

    void init(std::string s, int c_count, int t_count, int n=0)
    {
        _num_loops = n;
        _main_string = s;
        _string_size = s.size();
        _char_count = c_count;
        _thread_count = t_count;

        for(int i=0;i<_thread_count;i++)
        {
            std::binary_semaphore b(1);
            // indexUpdate_sema.push_back(b);
            // indexUpdate_sema[i].acquire();
            _index_is_updated.push_back(true);
            _next_start_index.push_back((i*_char_count)%_string_size);
        }

        _print_allowed.push_back(true);
        for(int i=1; i<_thread_count;i++)
        {
            _print_allowed.push_back(false);
        }
    }    

    std::binary_semaphore _print_sema(1);
    void criticalPrint(int thread_ID)
    {
        std::string subs;
        int count = 0;
        int pos = _next_start_index[thread_ID];
        while (count<_char_count)
        {
            subs+=_main_string[pos%_string_size];
            pos++;
            count++; 
        }

        std::cout<<subs<<std::endl;
    }

    void indexUpdate(int thread_ID)
    {
        if(!_index_is_updated[thread_ID])
        {            
            _next_start_index[thread_ID] = (_next_start_index[thread_ID]+(_thread_count*_char_count))%_string_size;
            _index_is_updated[thread_ID] = true;
        }        
    }

    void threadRunner(int thread_ID)
    {
        int count=0;
        while(count++<_num_loops)
        {
            //Tries to update index and prepare to extract next substring assigned to it:
            indexUpdate(thread_ID);

            if(_print_allowed[thread_ID] && _index_is_updated[thread_ID])
            {
                _print_sema.acquire();
                criticalPrint(thread_ID);
                _print_allowed[thread_ID]=false;
                _print_allowed[(thread_ID+1)%_thread_count]=true;//for sequential printing
                
                //indexUpdate_sema[index].release();
                _index_is_updated[thread_ID] = false;
                _print_sema.release();
            }
        }
    }
}

#endif