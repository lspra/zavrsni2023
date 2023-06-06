#include "../lexer/records.hpp"
#include "../tokenizer/tokenizer.hpp"
#include <fstream>
#include <stack>
class code_generator
{
protected:
	std::fstream file;
	std::string filename;
    Function* curr_function;
    bool inside_class;
public:
    Class* curr_class;
    code_generator(std::string filename_) : filename(filename_) {
		file.open(filename);
	}
    virtual void generate_undefined_exp(Var_object* var, Var_object* exp) = 0;
    // var = exp
    virtual void generate_exp(Var_object* var, Var_object* exp) = 0;
    // const
    virtual void generate_exp(Var_object* var, Token* t) = 0;
    // var_to = oper var_from
    virtual void generate_exp(Var_object* var_from, Var_object* var_to, Token* oper) = 0;
    // var_to = var_from1 oper var_from2
    virtual void generate_exp(Var_object* var_from1, Var_object* var_from2, Var_object* var_to, Token* oper) = 0;
    virtual void generateH(Var_object* var, Token* t) = 0;
    virtual void generateH(Var_object* var, Token* t, Var_object* exp) = 0;
    virtual void generate_var(Var_object* var) = 0;
    virtual void generate_var(Array* var) = 0;
    virtual void generate_if(Var_object* exp) = 0;
    virtual void generate_else() = 0;
    virtual void end_block() = 0;
    virtual void input(Var_object* var) = 0;
    virtual void print(Var_object* var) = 0;
    virtual void write_exp(std::string code) = 0;
    virtual void for_begin() = 0;
    virtual void for_continue() = 0;
    virtual void for_cond(Var_object* exp, std::string code) = 0;
    virtual void break_loop() = 0;
    virtual void continue_loop() = 0;
    virtual void return_() = 0;
    virtual void return_(Var_object* ret) = 0;
    virtual void function_decl(Function* f) = 0;
    virtual void main_decl(Function* f) = 0;
    void set_inside_class(bool inside_class_);
    void set_curr_function(Function* curr_function_);
    void set_curr_class(Class* curr_class_);
    void write(std::string code);
    virtual void generate_class(Class* c) = 0;
    virtual void function_call(Function* f, std::vector<Var_object*> args) = 0;
    virtual void constructor_return()=0;
};

class generate_C: public code_generator {
private:
    size_t for_numbs;
    std::stack<size_t> curr_fors;
public:
    generate_C(std::string name) : code_generator(name) {
        for_numbs = 0;
        file << "#include <stdio.h>\n";
    }
    virtual void generate_undefined_exp(Var_object* var, Var_object* exp);
    virtual void generate_exp(Var_object* var, Var_object* exp);
    virtual void generate_exp(Var_object* var, Token* t);
    virtual void generate_exp(Var_object* var_from, Var_object* var_to, Token* oper);
    virtual void generate_exp(Var_object* var_from1, Var_object* var_from2, Var_object* var_to, Token* oper);
    virtual void generateH(Var_object* var, Token* t);
    virtual void generateH(Var_object* var, Token* t, Var_object* exp);
    virtual void generate_var(Var_object* var);
    virtual void generate_var(Array* var);
    virtual void generate_if(Var_object* exp);
    void generate_if_not(Var_object* exp);
    virtual void generate_else();
    virtual void end_block();
    virtual void input(Var_object* var);
    virtual void print(Var_object* var);
    virtual void write_exp(std::string code);
    virtual void for_begin();
    virtual void for_continue();
    virtual void for_cond(Var_object* exp, std::string code);
    virtual void break_loop();
    virtual void continue_loop();
    virtual void return_();
    virtual void return_(Var_object* ret);
    virtual void function_decl(Function* f);
    virtual void main_decl(Function* f);
    virtual void generate_class(Class* c);
    virtual void function_call(Function* f, std::vector<Var_object*> args);
    virtual void constructor_return();
};