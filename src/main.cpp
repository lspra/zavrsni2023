#include"tokenizer/tokenizer.hpp"
#include<iostream>

int main(int argc, char** argv)
{
    std::string file = argv[1];
    tokenizer *t = new tokenizer(file);
    Token *a;
    do
    {
        a = t->get_next_token();
        std::cout << a->type << " " << a->line << " " << a->value << std::endl;
    } while(a->value.size() != 0);
}