#include <iostream>
#include <stack>
#include "lexer.hpp"

std::stack<Token*> tokens;
std::stack<Token*> temp_removed;

void remove_stack_top(int index) {
    for(int i = tokens.size() - index; i > 0; i--) {
        temp_removed.push(tokens.top());
        tokens.pop();
    }
}

void get_token(tokenizer* t) {
    if(temp_removed.size() != 0) {
        tokens.push(temp_removed.top());
        temp_removed.pop();
    } else {
        Token* token = t->get_next_token();
        tokens.push(token);
    }
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
}

void error_handle(std::string expected, int line) {
    std::cerr << "Expected " << expected << " in line " << line << std::endl;
    exit(1);
}

// no part of this lex should be read before calling this function
void base_list(tokenizer* t) {
    std::cout << "base list"  << std::endl;
    get_token(t);
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    if(tokens.top()->type != VAR) 
        error_handle("base class name", tokens.top()->line);
    get_token(t);
    if(tokens.top()->type == COMMA)
        base_list(t);
    if(tokens.top()->type != BRACKET_CLOSE)
        error_handle(")", tokens.top()->line);
}

// no part of this lex should be read before calling this function
void base_classes(tokenizer *t) {
    std::cout << "base classes"  << std::endl;
    get_token(t);
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    if(tokens.top()->type == CURLY_OPEN)
        return;
    if(tokens.top()->type != BRACKET_OPEN) 
        error_handle("(", tokens.top()->line);
    base_list(t);
    if(tokens.top()->type != BRACKET_CLOSE) 
        error_handle(")", tokens.top()->line);
    get_token(t);
}

// no part of this lex should be read before calling this function
void visibility_specifier(tokenizer* t) {
    std::cout << "visibility specifier"  << std::endl;
    get_token(t);
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    if(tokens.top()->value != "private" || tokens.top()->value != "public")
        error_handle("visibility specifier", tokens.top()->line);
}

// no part of this lex should be read before calling this function
void visibility_block(tokenizer* t) {
    std::cout << "visibility block"  << std::endl;
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    visibility_specifier(t);
    program_parts(t);
}

// no part of this lex should be read before calling this function
void class_body(tokenizer* t) {
    std::cout << "class body"  << std::endl;
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    visibility_block(t);
    get_token(t);
    if(tokens.top()->type != CURLY_CLOSE)
        class_body(t);
}

// keyword class should be read and saved in token before calling this function
void class_decl(tokenizer* t) {
    std::cout << "class decl"  << std::endl;
    get_token(t);
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    if(tokens.top()->type != VAR) 
        error_handle("class name", tokens.top()->line);
    base_classes(t);
    if(tokens.top()->type != CURLY_OPEN) 
        error_handle("{", tokens.top()->line);
    get_token(t);
    class_body(t);
    if(tokens.top()->type != CURLY_CLOSE) 
        error_handle("}", tokens.top()->line);

}

// first part of this lex should be read before calling this function
int decl_command(tokenizer* t) {
    std::cout << "decl command"  << std::endl;
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    if(tokens.top()->type != VAR)
        return -1;
    get_token(t);
    if(tokens.top()->value != "=")
        return -1;
    get_token(t);
    if(tokens.top()->type != DATA_TYPE)
        return -1;
    return 0;
}

// no part of this lex should be read before calling this function
void func_arguments_list(tokenizer* t) {
    std::cout << "func arguments list"  << std::endl;
    get_token(t);
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    if(decl_command(t) == -1)
        error_handle("declaration command", tokens.top()->line);
    get_token(t);
    if(tokens.top()->type == COMMA)
        func_arguments_list(t);
    if(tokens.top()->type != BRACKET_CLOSE)
        error_handle(")", tokens.top()->line);
}

void function_arguments(tokenizer* t) {
    get_token(t);
    if(tokens.top()->type == VAR) {
        func_arguments_list(t);
    }
    if(tokens.top()->type != BRACKET_CLOSE)
        error_handle(")", tokens.top()->line);
}

// first part of this lex should be read before calling this function
void return_type(tokenizer* t) {
    std::cout << "return type"  << std::endl;
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    if(tokens.top()->value != "=")
        return;
    get_token(t);
    if(tokens.top()->type != DATA_TYPE)
        error_handle("return type", tokens.top()->line);
}

void Lvalue(tokenizer* t) {
    std::cout << "Lvalue"  << std::endl;
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    if(tokens.top()->type != VAR)
        error_handle("Lvalue", tokens.top()->line);
    get_token(t);
    if(tokens.top()->type == SQUARE_OPEN) {
        if(exp(t) == -1)
            error_handle("expression", tokens.top()->line);
        if(tokens.top()->type != SQUARE_CLOSE)
            error_handle("]", tokens.top()->type);
    }
}

int A(tokenizer* t) {
    std::cout << "A"  << std::endl;
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    if(tokens.top()->type == VAR) {
        int stack_top = tokens.size();
        if(func_call(t) == -1) {
            remove_stack_top(stack_top);
            Lvalue(t);
        }
    } else if(tokens.top()->type == BRACKET_OPEN) {
        get_token(t);
        if(exp(t) == -1)
            return -1;
        if(tokens.top()->type != BRACKET_CLOSE) 
            error_handle(")", tokens.top()->line);
        get_token(t);
    } else if(tokens.top()->type == INT || tokens.top()->type == FLOAT || tokens.top()->type == STRING) {
        get_token(t);
    } else {
        return -1;
    }
    return 0;
}

int B(tokenizer* t) {
    std::cout << "B"  << std::endl;
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    if(tokens.top()->value == "+" || tokens.top()->value == "-" || tokens.top()->value == "~")
        get_token(t);
    return A(t);
}

int C(tokenizer* t) {
    std::cout << "C"  << std::endl;
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    if(B(t) == -1)
        return -1;
    if(tokens.top()->value == "<<" || tokens.top()->value == ">>") {
        get_token(t);
        return C(t);
    }
    return 0;
}

int D(tokenizer* t) {
    std::cout << "D"  << std::endl;
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    if(C(t) == -1)
        return -1;
    if(tokens.top()->value == "&") {
        get_token(t);
        return D(t);
    }
    return 0;
}

int E(tokenizer* t) {
    std::cout << "E"  << std::endl;
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    if(D(t) == -1)
        return -1;
    if(tokens.top()->value == "|" || tokens.top()->value == "^") {
        get_token(t);
        return E(t);
    }
    return 0;
}

int F(tokenizer* t) {
    std::cout << "F"  << std::endl;
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    if(E(t) == -1)
        return -1;
    if(tokens.top()->value == "*" || tokens.top()->value == "/" || tokens.top()->value == "%") {
        get_token(t);
        return F(t);
    }
    return 0;
}

int G(tokenizer* t) {
    std::cout << "G"  << std::endl;
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    if(F(t) == -1)
        return -1;
    if(tokens.top()->value == "+" || tokens.top()->value == "-") {
        get_token(t);
        return G(t);
    }
    return 0;
}

int H(tokenizer* t) {
    std::cout << "H"  << std::endl;
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    int stack_top = tokens.size();
    if(set_command(t) == -1) {
        remove_stack_top(stack_top);
        return G(t);
    }
    return 0;
}


int I(tokenizer* t) {
    std::cout << "I"  << std::endl;
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    if(H(t) == -1)
        return -1;
    if (tokens.top()->value == "==" || tokens.top()->value == "!=" || tokens.top()->value == "<" ||
         tokens.top()->value == "<=" || tokens.top()->value == ">" || tokens.top()->value == ">=") {
        get_token(t);
        return H(t);
    }
    return 0;
}

int J(tokenizer* t) {
    std::cout << "J"  << std::endl;
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    if(tokens.top()->value == "not")
        get_token(t);
    return I(t);
}

int K(tokenizer* t) {
    std::cout << "K"  << std::endl;
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    if(J(t) == -1)
        return -1;
    if(tokens.top()->value == "and") {
        get_token(t);
        return K(t);
    }
    return 0;
}

int exp(tokenizer* t) {
    std::cout << "exp"  << std::endl;
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    if(K(t) == -1)
        return -1;
    if(tokens.top()->value == "or" || tokens.top()->value == "xor") {
        get_token(t);
        return exp(t);
    }
    return 0;
}

void argument_list(tokenizer* t) {
    std::cout << "argument list"  << std::endl;
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    if(exp(t) == -1)
        error_handle("expression", tokens.top()->line);
    get_token(t);
    if(tokens.top()->type == COMMA) {
        get_token(t);
        if(tokens.top()->type == VAR)
            argument_list(t);
        else
            error_handle("Lvalue", tokens.top()->line);
    }
}

void arguments(tokenizer* t) {
    std::cout << "arguments"  << std::endl;
    get_token(t);
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    if(tokens.top()->type == VAR) {
        argument_list(t);
    }
    if(tokens.top()->type != BRACKET_CLOSE)
        error_handle(")", tokens.top()->line);
}

// returns -1 if what the function is parsing is not set_command
// error handling should be done after calling the function
int func_call(tokenizer* t) {
    std::cout << "function call"  << std::endl;
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    if(tokens.top()->type != VAR)
        return -1;
    get_token(t);
    if(tokens.top()->type == DOT) {
        get_token(t);
        if(tokens.top()->type != VAR)
            error_handle("function name", tokens.top()->line);
        get_token(t);
        if(tokens.top()->type != BRACKET_OPEN)
            error_handle("(", tokens.top()->line);
        arguments(t);
        if(tokens.top()->type != BRACKET_CLOSE)
            error_handle(")", tokens.top()->line);
        get_token(t);
    }
    if(tokens.top()->type != BRACKET_OPEN)
        return -1;
    arguments(t);
    if(tokens.top()->type != BRACKET_CLOSE)
        return -1;
    return 0;
}

// returns -1 if what the function is parsing is not set_command
// error handling should be done after calling the function
int set_command(tokenizer* t) {
    std::cout << "set command"  << std::endl;
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    if(tokens.top()->type == OPERATOR_MODIFY) {
        Lvalue(t);
    }
    int stack_top = tokens.size();
    if(tokens.top()->type == VAR) {
        Lvalue(t);
        if(tokens.top()->type == OPERATOR_MODIFY) {
            get_token(t);
            if(tokens.top()->type == BRACKET_OPEN) {
                get_token(t);
                if(exp(t) == -1)
                    error_handle("expression", tokens.top()->line);
                if(tokens.top()->type != BRACKET_CLOSE)
                    error_handle(")", tokens.top()->line);
                get_token(t);
            } else if(tokens.top()->type == INT || tokens.top()->type == FLOAT) {
                get_token(t);
            }
        } else if(tokens.top()->value == "=") {
            get_token(t);
            if(exp(t) == -1)
                return -1;
        } else {
            return -1;
        }
    } else if(tokens.top()->type == BRACKET_OPEN) {
        get_token(t);
        if(exp(t) == -1)
            return -1;
        get_token(t);
        if(tokens.top()->type != BRACKET_CLOSE)
            return -1;
        get_token(t);
        if(tokens.top()->type == OPERATOR_MODIFY) {
            get_token(t);
            Lvalue(t);
        } else
            return -1;
    } else if(tokens.top()->type == INT || tokens.top()->type == FLOAT) {
        get_token(t);
        if(tokens.top()->type == OPERATOR_MODIFY) {
            get_token(t);
            Lvalue(t);
        } else {
            return -1;
        }
    } else {
        return -1;
    }
    return 0;
}

void if_command(tokenizer* t) {
    std::cout << "if command"  << std::endl;
    get_token(t);
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    if(tokens.top()->type != BRACKET_OPEN)
        error_handle("(", tokens.top()->line);
    get_token(t);
    if(exp(t) == -1)
        error_handle("expression", tokens.top()->line);
    get_token(t);
    if(tokens.top()->type != BRACKET_CLOSE)
        error_handle(")", tokens.top()->line);
    block_commands(t);
    get_token(t);
    if(tokens.top()->value == "else")
        block_commands(t);
}

void for_command(tokenizer* t) {
    std::cout << "for command"  << std::endl;
    get_token(t);
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    if(tokens.top()->type != BRACKET_OPEN)
        error_handle("(", tokens.top()->line);
    get_token(t);
    int stack_top = tokens.size();
    if(set_command(t) == -1) {
        remove_stack_top(stack_top);
        if(decl_command(t) == -1)
            error_handle("command", tokens.top()->line);
        get_token(t);
        if(tokens.top()->type != SEMICOLON)
            error_handle(";", tokens.top()->line);
    }
    if(tokens.top()->type != SEMICOLON)
        error_handle(";", tokens.top()->line);
    get_token(t);
    if(exp(t) == -1)
        error_handle("expression", tokens.top()->line);
    get_token(t);
    if(set_command(t) == -1)
        error_handle("command", tokens.top()->line);
    if(tokens.top()->type != BRACKET_CLOSE)
        error_handle(")", tokens.top()->line);
    block_commands(t);
}

void while_command(tokenizer* t) {
    std::cout << "while command"  << std::endl;
    get_token(t);
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    if(tokens.top()->type != BRACKET_OPEN)
        error_handle("(", tokens.top()->line);
    get_token(t);
    if(exp(t) == -1)
        error_handle("expression", tokens.top()->line);
    get_token(t);
    if(tokens.top()->type != BRACKET_CLOSE)
        error_handle(")", tokens.top()->line);
    block_commands(t);
}

void input_command(tokenizer* t) {
    std::cout << "input command"  << std::endl;
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    get_token(t);
    if(tokens.top()->type != BRACKET_OPEN)
        error_handle("(", tokens.top()->line);
    get_token(t);
    Lvalue(t);
    if(tokens.top()->type != BRACKET_CLOSE)
        error_handle(")", tokens.top()->line);
    get_token(t);
}

void print_command(tokenizer* t) {
    std::cout << "print command"  << std::endl;
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    get_token(t);
    if(tokens.top()->type != BRACKET_OPEN)
        error_handle("(", tokens.top()->line);
    get_token(t);
    if(exp(t) == -1)
        error_handle("expression", tokens.top()->line);
    if(tokens.top()->type != BRACKET_CLOSE)
        error_handle(")", tokens.top()->line);
    get_token(t);
}

void return_command(tokenizer* t) {
    std::cout << "return command"  << std::endl;
    get_token(t);
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    if(tokens.top()->type != SEMICOLON) {
        if(exp(t) == -1)
            error_handle("expression", tokens.top()->line);
        get_token(t);
    }
}

void loop_command(tokenizer* t) {
    get_token(t);
}

// first part of this lex should be read before calling this function
void command(tokenizer* t) {
    std::cout << "command"  << std::endl;
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    if(tokens.top()->value == "if")
        if_command(t);
    else if(tokens.top()->value == "for") {
        for_command(t);
        get_token(t);
    }
    else if(tokens.top()->value == "while") {
        while_command(t);
        get_token(t);
    }
    else if(tokens.top()->value == "return")
        return_command(t);
    else {
        if(tokens.top()->value == "input")
            input_command(t);
        else if(tokens.top()->value == "print")
            print_command(t);
        else if(tokens.top()->value == "break" || tokens.top()->value == "continue")
            loop_command(t);
        else if(tokens.top()->type == VAR) {
            // decl_command, set_command, func_call
            int stack_top = tokens.size();
            if(set_command(t) == -1) {
                remove_stack_top(stack_top);
                if(decl_command(t) == -1) {
                    remove_stack_top(stack_top);
                    func_call(t);
                }
                get_token(t);
            }
        }
        if(tokens.top()->type != SEMICOLON)
            error_handle(";", tokens.top()->line);
        get_token(t);    
    }
}

// first part of this lex should be read before calling this function
void commands(tokenizer* t) {
    std::cout << "commands"  << std::endl;
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    if(tokens.top()->type == CURLY_CLOSE)
        return;
    command(t);
    commands(t);
}

// no part of this lex should be read before calling this function
void block_commands(tokenizer* t) {
    std::cout << "block commands"  << std::endl;
    get_token(t);
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    if(tokens.top()->type == CURLY_OPEN) {
        get_token(t);
        commands(t);
        if(tokens.top()->type != CURLY_CLOSE)
            error_handle("}", tokens.top()->line);
    } else {
        get_token(t);
        command(t);
    }

}

// function name should be read and saved in token before calling this function
void function(tokenizer* t) {
    std::cout << "function"  << std::endl;
    get_token(t);
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    if(tokens.top()->type != BRACKET_OPEN)
        error_handle("(", tokens.top()->line);
    function_arguments(t);
    if(tokens.top()->type != BRACKET_CLOSE) 
        error_handle(")", tokens.top()->line);
    return_type(t);
    block_commands(t);
}

// no part of this lex should be read before calling this function
void program_parts(tokenizer* t) {
    std::cout << "program parts"  << std::endl;
    get_token(t);
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    if(tokens.top()->type == KEYWORD && tokens.top()->value == "main")
        return;
    if(tokens.top()->type == KEYWORD && tokens.top()->value == "class")
        class_decl(t);
    if(tokens.top()->type == VAR)
        function(t);
}

// keyword main should be read and saved in token before calling this function
void main_function(tokenizer* t) {
    std::cout << "main function"  << std::endl;
    get_token(t);
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    if(tokens.top()->type != BRACKET_OPEN) 
        error_handle("(", tokens.top()->line);
    function_arguments(t);
    if(tokens.top()->type != BRACKET_CLOSE) 
        error_handle(")", tokens.top()->line);
    block_commands(t);
}

// no part of this lex should be read before calling this function
void program(tokenizer* t) {
    std::cout << "program"  << std::endl;
    program_parts(t);
    main_function(t);
    get_token(t);
    if(tokens.top()->type != END_OF_FILE)
        error_handle("EOF", tokens.top()->line);
}