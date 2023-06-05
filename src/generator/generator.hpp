#include "../lexer/records.hpp"
#include "../tokenizer/tokenizer.hpp"
#include <fstream>
#include <stack>
class code_generator
{
protected:
	std::fstream file;
	std::string filename;
public:
    code_generator(std::string filename_) : filename(filename_) {
		file.open(filename);
	}
public:
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
    virtual void while_begin() = 0;
    virtual void while_branch(Var_object* exp) = 0;
    virtual void while_end() = 0;
    virtual void input(Var_object* var) = 0;
    virtual void print(Var_object* var) = 0;
};

class generate_C: public code_generator {
private:
    size_t while_numbs;
    std::stack<size_t> curr_whiles;
public:
    generate_C(std::string name) : code_generator(name) {
        while_numbs = 0;
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
    virtual void while_begin();
    virtual void while_branch(Var_object* exp);
    virtual void while_end();
    virtual void input(Var_object* var);
    virtual void print(Var_object* var);
};