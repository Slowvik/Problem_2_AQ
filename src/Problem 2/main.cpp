#include "sequential.h"
#include "sema.h"
#include "max_threading.h"
#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <chrono>

int main(int argc, char* argv[])
{
    int n_loops = 100000;
    std::string s = argv[1];
    int c_count = atoi(argv[2]);
    int t_count = atoi(argv[3]);

    std::cout<<"Using "<<t_count<<" threads to print "<<s<<" alternatively in sets of "<<c_count<<" chars"<<std::endl;

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

    //Initialising the different algorithms
    seq::init(s, c_count, n_loops);
    sema::init(s, c_count, t_count, n_loops);
    max_threading::init(s, c_count, t_count, n_loops);

    //vector of threads
    std::vector<std::thread> thread_vector_sema;
    std::vector<std::thread> thread_vector_max_threading;

    //Baseline: Time taken to print sequentially:
    auto start_time_seq = std::chrono::high_resolution_clock::now();
    seq::print_seq();
    auto end_time_seq = std::chrono::high_resolution_clock::now(); 

    //Time taken by sema.h:
    auto startTimeSema = std::chrono::high_resolution_clock::now();
    for(int i = 0; i<t_count;i++)
    {
        std::thread th(sema::threadRunner, i); //running code with semaphore
        thread_vector_sema.push_back(move(th)); //Copy constructor of std::thread is deleted, have to use move() instead
    }
    for(int i = 0; i<t_count; i++)
    {
        thread_vector_sema[i].join();
    }
    auto endTimeSema = std::chrono::high_resolution_clock::now();
    
    //Time taken by max_threading:
    auto startTimeMaxThreading = std::chrono::high_resolution_clock::now();
    for(int i = 0; i<t_count;i++)
    {
        std::thread th(max_threading::threadRunner, i);//running code with max_threading
        thread_vector_max_threading.push_back(move(th)); //Copy constructor of std::thread is deleted, have to use move() instead
    }
    for(int i = 0; i<t_count; i++)
    {
        thread_vector_max_threading[i].join();
    }
    auto endTimeMaxThreading = std::chrono::high_resolution_clock::now();

    std::cout<<"\n\n\n\n";

    std::cout<<"Time taken for sequential printing without threading is "<<std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(end_time_seq - start_time_seq).count()<<" milliseconds"<<std::endl;
    std::cout<<"Time taken by sema is "<<std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(endTimeSema - startTimeSema).count()<<" milliseconds"<<std::endl;
    std::cout<<"Time taken by max_threading is "<<std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(endTimeMaxThreading - startTimeMaxThreading).count()<<" milliseconds"<<std::endl;


    system("PAUSE");
    return 0;
}