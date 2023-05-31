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
	string_type,
	class_,
	function_,
	class_instance
};

class Scope;

class Variable {
	public:
		std::string name;
		data_types type;
		virtual Variable* var_extend(tokenizer* t, Scope* scope) = 0;
};

class Var_object: public Variable {
	public:
		virtual Variable* var_extend(tokenizer* t, Scope* scope) = 0;
};

class Scope {
	public:
		std::vector <Variable*> variables;
		Scope* parent_scope;
		Scope(Scope* parent_scope_): parent_scope(parent_scope_) {}
};
class Function: public Variable {
	public:
		Var_object* return_object;
		Scope* function_scope;
		std::vector <Variable* > function_parameters;
		Function(std::string name_, Scope* scope, Var_object* return_object_): function_scope(scope) {
			name = name_;
			type = function_;
			return_object = return_object_;
		}
		Function(std::string name_, Scope* scope): function_scope(scope) {
			name = name_;
			type = function_;
		}
		virtual Variable* var_extend(tokenizer* t, Scope* scope);
};
class Class: public Variable {
	public:
		Scope* class_scope;
		Function* constructor;
		Class(std::string name_, Scope* scope) {
			name = name_;
			constructor = nullptr;
			type = class_;
			class_scope = scope;
		}
		virtual Variable* var_extend(tokenizer* t, Scope* scope);
};

class Simple_Variable: public Var_object {
	public:
		Simple_Variable(std::string name_, data_types type_) {
			this->name = name_;
			this->type = type_;
		}
		virtual Variable* var_extend(tokenizer* t, Scope* scope) {
			return this;
		}
};
class Class_instance: public Var_object {
	public:
		Class* class_type;
		Class_instance(std::string name_, Class* class_): class_type(class_) {
			this->name = name_;
			type = class_instance;
		}
		virtual Variable* var_extend(tokenizer* t, Scope* scope);
};
class array: public Var_object {
	public:
		size_t dimension;
		// TODO
		// in current implementation, last element of array is size of "first" array
		std::vector<size_t> size;
		array(std::string name, size_t dimension, std::vector<size_t> size_);
		virtual Variable* var_extend(tokenizer* t, Scope* scope);
};