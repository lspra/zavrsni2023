#include "../tokenizer/tokenizer.hpp"
#include <string>
#include <vector>

class Variable {
	public:
		Token* token;
		int type;
		// initial_value;
		bool operator== (const Variable& other) {
			return this->token->value == other.token->value;
		}
};
class Class;
class Function;
class Variable;

class Scope {
	public:
		std::vector <Variable> variables;
		std::vector <Class> classes;
		std::vector <Function*> functions;
		Scope* parent_scope;
};

class Function: public Variable {
	public:
		Scope function_scope;
		std::vector <Variable> arguments;
};

class Class: public Variable {
	public:
		Scope* class_scope;
};