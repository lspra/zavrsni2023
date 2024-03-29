#include "../generator/generator.hpp"
#include "../tokenizer/tokenizer.hpp"
#include "records.hpp"
#include <stack>

struct H0_args {
    Var_object* var;
    Token* t;
};

struct H1_args {
    Var_object* var;
    Token* t;
    Var_object* exp;
};

class lexer;
class lexer
{
private:
    bool inside_class;
    Function* curr_function;
    Variable* find_var_all_scopes(Scope* cur_scope);
    Variable* var_extend(Variable* v, Scope* scope);
    Variable* var_extend(Function* v, Scope* scope, Var_object* first_arg);
    Variable* var_extend(Class* v, Scope* scope);
    Variable* var_extend(Var_object* v, Scope* scope);
    Variable* var_extend(Array* v, Scope* scope);
    Variable* var_extend(Array_element* v, Scope* scope);
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
    std::vector<Var_object*> argument_list(Scope* scope, std::vector<Var_object*> *arguments_vector, size_t index);
    Var_object* decl_command(Scope* scope, bool func_arg);
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
    void return_type(Function* function);
    void function(Scope* scope);
    void base_list(Scope* scope);
    void visibility_block(Scope* scope);
    void class_body(Scope* scope);
    void class_decl(Scope* scope);
    void program_parts(Scope* scope);
    void main_function(Scope* scope);
    void base_classes(Scope* scope);
    void loop_command();
    std::stack<H0_args> H0_arguments;
    std::stack<H1_args> H1_arguments;
public:
    Scope* global_scope;
    std::vector<Var_object*> arguments(Scope* scope, std::vector<Var_object*> *arguments_vector);
    Var_object* exp(Scope* scope);
    void program();
    tokenizer* t;
    code_generator* g;
    lexer(tokenizer* t_, code_generator* g_): t(t_), g(g_) {
        inside_class = false;
    }
};