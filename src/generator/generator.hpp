#include "../lexer/records.hpp"
#include<fstream>
#include "../tokenizer/tokenizer.hpp"
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
    // var = exp
    virtual void generateA(Var_object* var, Var_object* exp) = 0;
    // const
    virtual void generateA(Var_object* var, Token* t) = 0;
    virtual void generateB(Var_object* var_from, Var_object* var_to, Token* oper) = 0;
    virtual void generateC(Var_object* var_from1, Var_object* var_from2, Var_object* var_to, Token* oper) = 0;
    virtual void generateH(Var_object* var, Token* t) = 0;
    virtual void generateH(Var_object* var, Token* t, Var_object* exp) = 0;
};

class generate_C: public code_generator {
public:
    generate_C(std::string name) : code_generator(name) {
    }
    virtual void generateA(Var_object* var, Var_object* exp);
    virtual void generateA(Var_object* var, Token* t);
    virtual void generateB(Var_object* var_from, Var_object* var_to, Token* oper);
    virtual void generateC(Var_object* var_from1, Var_object* var_from2, Var_object* var_to, Token* oper);
    virtual void generateH(Var_object* var, Token* t);
    virtual void generateH(Var_object* var, Token* t, Var_object* exp);
};