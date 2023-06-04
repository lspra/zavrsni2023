#include<iostream>
#include "lexer/lexer.hpp"

int main(int argc, char** argv)
{
	std::string file = argv[1];
	tokenizer t(file);
	lexer l(&t, nullptr);
	l.program();
	std::cout << "Parsing finished" << std::endl;
}