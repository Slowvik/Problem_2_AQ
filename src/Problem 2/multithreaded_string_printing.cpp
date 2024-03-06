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

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <string>
#include <vector>
#include <atomic>

namespace printer
{
    std::mutex main_mutex;
    std::condition_variable main_cond;

    std::vector<std::thread> t;
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
        int pos = printer::start_index;
        while (count<char_count)
        {
            subs+=printer::main_string[pos%printer::string_size];
            pos++;
            count++; 
        }
        return subs;
    }

    //Calculates starting position of next substring
    void calc_next_start_index()
    {
        printer::start_index = (printer::start_index+printer::char_count)%printer::string_size;
    }


    //thread_runner currently prints the substrings in an infinite loop
    void thread_runner(int index)
    {
        while(1)
        {
            //acquire the mutex
            std::unique_lock<std::mutex> thread_lock(main_mutex);

            //condition_variable atomically releases the mutex till predicate is met (till it's turn comes)
            main_cond.wait(thread_lock, [&]{return turn[index];});

            //Printing the substring
            std::cout<<printer::return_substring()<<std::endl; 

            //calculate start index for next thread
            printer::calc_next_start_index();           

            //Change own turn[index] to false;
            turn[index] = false;

            //Change next thread's turn[(index+1)%thread_count] to true - results in threads being called sequentially.
            turn[(index+1)%printer::thread_count] = true;

            //release the mutex
            thread_lock.unlock();

            //notify waiting threads
            main_cond.notify_all();
        }
    }
}




int main(int argc, char* argv[])
{
    std::string s = argv[1];
    int c_count = atoi(argv[2]);
    int t_count = atoi(argv[3]);

    std::cout<<"Using "<<t_count<<" threads to print "<<s<<" alternatively in sets of "<<c_count<<" chars"<<std::endl;

    printer::main_string = s;
    printer::char_count = c_count;
    printer::string_size = s.size();
    printer::thread_count = t_count;

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
    printer::turn.push_back(true);
    for(int i=1; i<t_count; i++)
    {
        printer::turn.push_back(false);
    }

    //Initialise the threads
    for(int i = 0; i<t_count;i++)
    {
        std::thread th(printer::thread_runner, i); //Initialise a thread with a thread_runner + thread ID (basically its array index)
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