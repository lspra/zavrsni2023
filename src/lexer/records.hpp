#pragma once
#include<vector>
#include<string>
#include<unordered_set>

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
	class_instance,
	array_,
	array_element
};

class Scope;
class Var_object;
class Variable {
	private:
        static inline int generated;
    public:
		std::string name;
		std::string generated_name;
		std::string generated_code;
		data_types type;
		bool generate_prefix;
	    Variable();
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
		std::vector <Var_object* > function_parameters;
		Function(std::string name_, Scope* scope, Var_object* return_object_);
		Function(std::string name_, Scope* scope);
};

class Class: public Variable {
	public:
		Scope* class_scope;
		Function* constructor;
		std::unordered_set<Class*> base_classes;
		Class(std::string name_, Scope* scope);
};

class Var_object: public Variable {
	public:
		Class* class_type;
		Var_object(std::string name_, data_types type_);
		virtual data_types get_type();
		std::string get_name(Class* class_, Function* function_);
		std::string name_prefix;
};
class Array: public Variable {
	public:
		std::vector<Var_object*> size;
		Var_object* containing_type;
		Array(std::string name_, std::vector<Var_object*> size_, Var_object* type);
};

class Array_element: public Var_object {
	private:
		std::vector<Var_object*> begin_index;
		std::vector<Var_object*> end_index;
	public:
		void add_begin(Var_object* begin);
		void add_end(Var_object* end);
		Array* array_;
		Array_element(Array_element* arr);
		Array_element(Array* arr);
		bool is_one_element();
		virtual data_types get_type();
};