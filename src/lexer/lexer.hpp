#include "../generator/generator.hpp"
#include "../tokenizer/tokenizer.hpp"
#include <vector>
#include <string>

class lexer;

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
		data_types type;
		virtual Variable* var_extend(lexer* l, Scope* scope) = 0;
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
		virtual Variable* var_extend(lexer* l, Scope* scope);
};

class Class: public Variable {
	public:
		Scope* class_scope;
		Function* constructor;
		std::unordered_set<Class*> base_classes;
		Class(std::string name_, Scope* scope);
		virtual Variable* var_extend(lexer* l, Scope* scope);
};

class Var_object: public Variable {
	public:
		Class* class_type;
		Var_object(std::string name_, data_types type_);
		virtual Variable* var_extend(lexer* l, Scope* scope);
};
class Array: public Variable {
	public:
		std::vector<Var_object*> size;
		Var_object* containing_type;
		Array(std::string name_, std::vector<Var_object*> size_, Var_object* type);
		virtual Variable* var_extend(lexer* l, Scope* scope);
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
		virtual Variable* var_extend(lexer* l, Scope* scope);
		bool is_one_element();
};

class lexer
{
private:
    Function* curr_function;
    Var_object* var(Scope* scope, bool* Lvalue);
    Var_object* Lvalue(Scope* scope);
    Var_object* A(Scope* scope);
    Var_object* B(Scope* scope);
    Var_object* C(Scope* scope);
    Var_object* D(Scope* scope);
    Var_object* E(Scope* scope);
    Var_object* F(Scope* scope);
    Var_object* G(Scope* scope);
    Var_object* H(Scope* scope);
    Var_object* I(Scope* scope);
    Var_object* J(Scope* scope);
    Var_object* K(Scope* scope);
    void argument_list(Scope* scope, std::vector<Var_object*> *arguments_vector, size_t index);
    bool decl_command(Scope* scope);
    void if_command(Scope* scope);
    void for_command(Scope* scope);
    void while_command(Scope* scope);
    void input_command(Scope* scope);
    void print_command(Scope* scope);
    void return_command(Scope* scope);
    void command(Scope* scope);
    void commands(Scope* scope);
    void block_commands(Scope* scope);
    void func_arguments_list(Scope* scope, Function* function);
    void function_arguments(Scope* scope, Function* function);
    void return_type(Scope* scope, Function* function);
    void function(Scope* scope);
    void base_list(Scope* scope);
    void visibility_block(Scope* scope);
    void class_body(Scope* scope);
    void class_decl(Scope* scope);
    void program_parts(Scope* scope);
    void main_function(Scope* scope);
    void base_classes(Scope* scope);
public:
    Scope* global_scope;
    void arguments(Scope* scope, std::vector<Var_object*> *arguments_vector);
    Var_object* exp(Scope* scope);
    void program();
    tokenizer* t;
    code_generator* g;
    lexer(tokenizer* t_, code_generator* g_): t(t_), g(g_) {

    }
};