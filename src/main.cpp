#include<iostream>
#include "lexer/lexer.hpp"

int main(int argc, char** argv)
{
    std::string file = argv[1];
    tokenizer *t = new tokenizer(file);
    program(t);
}