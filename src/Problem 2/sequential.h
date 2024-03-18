#include <iostream>
#include <string>

namespace seq
{
    std::string main_string;
    int main_string_length;
    int char_count;
    int num_loops;
    int start_pos;
    std::string subs;
    int pos;


    void init(std::string s, int c_count, int n_loops)
    {
        main_string = s;
        main_string_length = s.length();
        char_count = c_count;
        num_loops = n_loops;

        pos = 0;
        subs = "";

    }

    void print_seq()
    {
        int count = 0;
        while(count++<num_loops)
        {
            subs = "";

            for(int i = 0; i<char_count; i++)
            {                
                subs += main_string[(pos+i)%main_string_length];
            }

            pos = (pos+char_count)%main_string_length;
            std::cout<<subs<<std::endl;
        }
    }
}