#include<iostream>
#include "lexer/lexer.hpp"

int main(int argc, char** argv)
{
	std::string source = argv[1];
	std::string confidant = argv[2];
	tokenizer t(source);
	generate_C gc(confidant);
	lexer l(&t, &gc);
	l.program();
	std::cout << "Parsing finished" << std::endl;
}