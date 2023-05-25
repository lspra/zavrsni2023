#include "../tokenizer/tokenizer.hpp"
#include <string>
#include <vector>

enum data_types {
	undefined = -1,
	bool_type,
	int8,
	int16,
	int32,
	int64,
	uint8,
	uint16,
	uint32,
	uint64,
	float32,
	float64,
	float96,
	string_type
};

class Scope;

class Variable {
	public:
		Token* token;
		data_types type;
		virtual Variable* var_extend(tokenizer* t, Scope* scope) = 0;
};
class Scope {
	public:
		std::vector <Variable*> variables;
		Scope* parent_scope;
		Scope(Scope* parent_scope_): parent_scope(parent_scope_) {}
};
class Simple_Variable: public Variable {
	public:
		data_types type;
		Simple_Variable(Token* token_, data_types type_): type(type_) {
			this->token = token_;
		}
		virtual Variable* var_extend(tokenizer* t, Scope* scope) {
			return this;
		}
};
class Function: public Variable {
	public:
		// TODO return type
		Scope* function_scope;
		std::vector <Variable* > function_parameters;
		Function(Token* token_, Scope* scope): function_scope(scope) {
			this->token = token_;
		}
		virtual Variable* var_extend(tokenizer* t, Scope* scope);
};
class Class: public Variable {
	public:
		Scope* class_scope;
		Function* constructor;
		Class(Token* token_, Scope* scope): class_scope(scope) {
			this->token = token_;
			constructor = NULL;
		}
		virtual Variable* var_extend(tokenizer* t, Scope* scope);
};
class Class_instance: public Variable {
	public:
		Class* class_type;
		Class_instance(Token* token_, Class* class_): class_type(class_) {
			this->token = token_;
		}
		virtual Variable* var_extend(tokenizer* t, Scope* scope);
};
class array: public Variable {
	public:
		size_t dimension;
		// in current implementation, last element of array is size of "first" array
		std::vector<size_t> size;
		array(Token* token_, size_t dimension, std::vector<size_t> size_);
		virtual Variable* var_extend(tokenizer* t, Scope* scope);
};