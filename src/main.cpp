#include<iostream>
#include "lexer/lexer.hpp"

int main(int argc, char** argv)
{
	std::string source;
	if(argc >= 2)
		source = argv[1];
	else {
		std::cerr << "no argument";
		exit(1);
	}
	std::string confidant = "a.c";
	if(argc >= 3)
		confidant = argv[2];
	tokenizer t(source);
	generate_C *gc = new generate_C(confidant);
	lexer l(&t, gc);
	l.program();
	// std::cout << "Parsing finished" << std::endl;
}