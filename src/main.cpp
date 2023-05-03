#include<iostream>
#include<fstream>
#include"tokenizer/tokenizer.hpp"
#include<string>
int num_line = 0;



int main(int argc, char** argv)
{
    std::string file = argv[1];
    tokenizer *t = new tokenizer(file);
    while(true)
    {
        Token *a = t->get_next_token();
        std::cout << a->type << " " << a->value << std::endl;
    }
}