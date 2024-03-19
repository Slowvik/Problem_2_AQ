#include <iostream>
#include <string>

namespace seq
{
    std::string _main_string;
    int _main_string_length;
    int _char_count;
    int _num_loops;
    int _start__pos;
    std::string _subs;
    int _pos;


    void init(std::string s, int c_count, int n_loops)
    {
        _main_string = s;
        _main_string_length = s.length();
        _char_count = c_count;
        _num_loops = n_loops;

        _pos = 0;
        _subs = "";

    }

    void printSeq()
    {
        int count = 0;
        while(count++<_num_loops)
        {
            _subs = "";

            for(int i = 0; i<_char_count; i++)
            {                
                _subs += _main_string[(_pos+i)%_main_string_length];
            }

            _pos = (_pos+_char_count)%_main_string_length;
            std::cout<<_subs<<std::endl;
        }
    }
}