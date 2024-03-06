#include "mutex_condition.h"
#include "semaphore_cpp20.h"
#include <iostream>
#include <thread>
#include <vector>
#include <string>

int main(int argc, char* argv[])
{
    std::string s = argv[1];
    int c_count = atoi(argv[2]);
    int t_count = atoi(argv[3]);

    std::cout<<"Using "<<t_count<<" threads to print "<<s<<" alternatively in sets of "<<c_count<<" chars"<<std::endl;

    mut_cond::main_string = s;
    mut_cond::char_count = c_count;
    mut_cond::string_size = s.size();
    mut_cond::thread_count = t_count;

    sema::main_string = s;
    sema::char_count = c_count;
    sema::start_index = -c_count;
    sema::string_size = s.size();
    sema::thread_count = t_count;
    //sema::print_sema(t_count);

    if(s=="")
    {
        std::cout<<"Please enter a non-empty string"<<std::endl;
        return 1;
    }
    if(c_count<1)
    {
        std::cout<<"Please enter a valid character count (>=1)"<<std::endl;
        return 1;
    }
    if(t_count<1)
    {
        std::cout<<"Please enter a valid number of threads (>=1)"<<std::endl;
        return 1;
    }

    //vector of threads
    std::vector<std::thread> thread_vector;

    //Initialise the turns: at start, only thread[0] should be allowed to print.
    // mut_cond::turn.push_back(true);
    // for(int i=1; i<t_count; i++)
    // {
    //     mut_cond::turn.push_back(false);
    // }

    //std::cout<<"Thread count: "<<sema::thread_count<<std::endl;

    //Initialise the threads
    for(int i = 0; i<t_count;i++)
    {
        //std::thread th(mut_cond::thread_runner, i); //running code with mutex and cv
        std::thread th(sema::thread_runner, i); //running code with semaphore
        thread_vector.push_back(move(th)); //Copy constructor of std::thread is deleted, have to use move() instead
    }

    //join the threads
    for(int i = 0; i<t_count; i++)
    {
        thread_vector[i].join();
    }

    system("PAUSE");
    return 0;
}