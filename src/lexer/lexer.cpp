#include <iostream>
#include <stack>
#include "./tokenizer/tokenizer.hpp"

std::stack<Token*> tokens;

void remove_stack_top(int index) {
    for(int i = tokens.size() - index; i > 0; i--)
        tokens.pop();
    // this should also reset tokenizer, think about how that should be done
}

void get_token(tokenizer* t) {
    tokens.push(t->get_next_token());
}

void error_handle(std::string expected, int line) {
    std::cerr << "Expected " << expected << " in line" << line << std::endl;
    exit(1);
}

// no part of this lex should be read before calling this function
void base_list(tokenizer* t) {
    get_token(t);
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
    get_token(t);
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
    get_token(t);
    if(tokens.top()->value != "private" || tokens.top()->value != "public")
        error_handle("visibility specifier", tokens.top()->line);
}

// no part of this lex should be read before calling this function
void visibility_block(tokenizer* t) {
    visibility_specifier(t);
    program_parts(t);
}

// no part of this lex should be read before calling this function
void class_body(tokenizer* t) {
    visibility_block(t);
    get_token(t);
    if(tokens.top()->type != CURLY_CLOSE)
        class_body(t);
}

// keyword class should be read and saved in token before calling this function
void class_decl(tokenizer* t) {
    get_token(t);
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
    if(tokens.top()->type != VAR)
        return -1;
    get_token(t);
    if(tokens.top()->value != "=")
        return -1;
    get_token(t);
    if(tokens.top()->type != DATA_TYPE)
        return -1;
}

// no part of this lex should be read before calling this function
void function_arguments(tokenizer* t) {
    get_token(t);
    if(decl_command(t) == -1)
        error_handle("declaration command", tokens.top()->line);
    get_token(t);
    if(tokens.top()->type == COMMA)
        function_arguments(t);
    if(tokens.top()->type != BRACKET_CLOSE)
        error_handle(")", tokens.top()->line);
}

// no part of this lex should be read before calling this function
void return_type(tokenizer* t) {
    get_token(t);
    if(tokens.top()->value != "=")
        return;
    get_token(t);
    if(tokens.top()->type != DATA_TYPE)
        error_handle("return type", tokens.top()->line);
}

void Lvalue(tokenizer* t) {
    if(tokens.top()->type != VAR)
        error_handle("Lvalue", tokens.top()->line);
    get_token(t);
    if(tokens.top()->type == SQUARE_OPEN) {
        exp(t);
        if(tokens.top()->type != SQUARE_CLOSE)
            error_handle("]", tokens.top()->type);
    }
}

void A(tokenizer* t) {
    if(tokens.top()->type == VAR)
        Lvalue(t);
    else if(tokens.top()->type == BRACKET_OPEN) {
        exp(t);
        get_token(t);
        if(tokens.top()->type != BRACKET_CLOSE) 
            error_handle(")", tokens.top()->line);
    } else if(tokens.top()->type == INT || tokens.top()->type == FLOAT || tokens.top()->type == STRING) {

    } else {
        error_handle("expression", tokens.top()->line);
    }
}

void B(tokenizer* t) {
    if(tokens.top()->value == "+" || tokens.top()->value == "-" || tokens.top()->value == "~")
        get_token(t);
    A(t);
}

void C(tokenizer* t) {
    B(t);
    get_token(t);
    if(tokens.top()->value == "<<" || tokens.top()->value == ">>") {
        get_token(t);
        B(t);
    }
}

void D(tokenizer* t) {
    C(t);
    get_token(t);
    if(tokens.top()->value == "&") {
        get_token(t);
        C(t);
    }
}

void E(tokenizer* t) {
    D(t);
    get_token(t);
    if(tokens.top()->value == "|" || tokens.top()->value == "^") {
        get_token(t);
        D(t);
    }
}

void F(tokenizer* t) {
    E(t);
    get_token(t);
    if(tokens.top()->value == "*" || tokens.top()->value == "/" || tokens.top()->value == "%") {
        get_token(t);
        E(t);
    }
}

void G(tokenizer* t) {
    F(t);
    get_token(t);
    if(tokens.top()->value == "+" || tokens.top()->value == "-") {
        get_token(t);
        F(t);
    }
}

void H(tokenizer* t) {
    if(tokens.top()->type == OPERATOR_MODIFY) {
        Lvalue(t);
    }
    int stack_top = tokens.size();
    if(tokens.top()->type == VAR) {
        Lvalue(t);
        get_token(t);
        if(tokens.top()->type == OPERATOR_MODIFY) {
            get_token(t);
            if(tokens.top()->type == BRACKET_OPEN) {
                exp(t);
                get_token(t);
                if(tokens.top()->type != BRACKET_CLOSE)
                    error_handle(")", tokens.top()->line);
                get_token(t);
            } else if(tokens.top()->type == INT || tokens.top()->type == FLOAT) {
                get_token(t);
            }
        } else if(tokens.top()->value == "=") {
            exp(t);
        } else {
            remove_stack_top(stack_top);
            G(t);
        }
    } else if(tokens.top()->type == BRACKET_OPEN) {
        get_token(t);
        exp(t);
        get_token(t);
        if(tokens.top()->type != BRACKET_CLOSE)
            error_handle(")", tokens.top()->line);
        get_token(t);
        if(tokens.top()->type == OPERATOR_MODIFY) {
            get_token(t);
            Lvalue(t);
        } else {
            remove_stack_top(stack_top);
            G(t);
        }
    } else if(tokens.top()->type == INT || tokens.top()->type == FLOAT) {
        get_token(t);
        if(tokens.top()->type == OPERATOR_MODIFY) {
            get_token(t);
            Lvalue(t);
        } else {
            remove_stack_top(stack_top);
            G(t);
        }
    }
}


void I(tokenizer* t) {
    H(t);
    get_token(t);
    if (tokens.top()->value == "==" || tokens.top()->value == "!=" || tokens.top()->value == "<" || tokens.top()->value == "<=" || tokens.top()->value == ">" || ">=") {
        get_token(t);
        H(t);
    }
}

void J(tokenizer* t) {
    if(tokens.top()->value == "not")
        get_token(t);
    I(t);
}

void K(tokenizer* t) {
    J(t);
    get_token(t);
    if(tokens.top()->value == "and") {
        get_token(t);
        J(t);
    }
}

void exp(tokenizer* t) {
    K(t);
    get_token(t);
    if(tokens.top()->value == "or" || tokens.top()->value == "xor") {
        get_token(t);
        K(t);
    }
}

void argument_list(tokenizer* t) {
    Lvalue(t);
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
    get_token(t);
    if(tokens.top()->type == VAR) {
        argument_list(t);
    }
    if(tokens.top()->type != BRACKET_CLOSE)
        error_handle(")", tokens.top()->line);
}

void func_call(tokenizer* t) {
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
    }
    get_token(t);
    if(tokens.top()->type != BRACKET_OPEN)
        error_handle("(", tokens.top()->line);
    arguments(t);
    if(tokens.top()->type != BRACKET_CLOSE)
        error_handle(")", tokens.top()->line);

}

// returns -1 if what the function is parsing is not set_command
// error handling should be done after calling the function
int set_command(tokenizer* t) {
    Lvalue(t);
    if(tokens.top()->value != "=") 
        return -1;
    get_token(t);
    int stack_top = tokens.size();
    exp(t);
    if(tokens.top()->type != SEMICOLON && tokens.top()->type != BRACKET_CLOSE) {
        remove_stack_top(stack_top);
        func_call(t);
    }
    return 0;
}

void if_command(tokenizer* t) {
    get_token(t);
    if(tokens.top()->type != BRACKET_OPEN)
        error_handle("(", tokens.top()->line);
    get_token(t);
    exp(t);
    get_token(t);
    if(tokens.top()->type != BRACKET_CLOSE)
        error_handle(")", tokens.top()->line);
    block_commands(t);
    get_token(t);
    if(tokens.top()->value == "else")
        block_commands(t);
}

void for_command(tokenizer* t) {
    get_token(t);
    if(tokens.top()->type != BRACKET_OPEN)
        error_handle("(", tokens.top()->line);
    if(set_command(t) == -1)
        error_handle("command", tokens.top()->line);
    get_token(t);
    exp(t);
    if(set_command(t) == -1)
        error_handle("command", tokens.top()->line);
    get_token(t);
    if(tokens.top()->type != BRACKET_CLOSE)
        error_handle(")", tokens.top()->line);
    block_commands(t);
}

void while_command(tokenizer* t) {
    get_token(t);
    if(tokens.top()->type != BRACKET_OPEN)
        error_handle("(", tokens.top()->line);
    get_token(t);
    exp(t);
    get_token(t);
    if(tokens.top()->type != BRACKET_CLOSE)
        error_handle(")", tokens.top()->line);
    block_commands(t);
}

void input_command(tokenizer* t) {
    if(tokens.top()->type != BRACKET_OPEN)
        error_handle("(", tokens.top()->line);
    Lvalue(t);
    get_token(t);
    if(tokens.top()->type != BRACKET_CLOSE)
        error_handle(")", tokens.top()->line);
}

void print_command(tokenizer* t) {
    if(tokens.top()->type != BRACKET_OPEN)
        error_handle("(", tokens.top()->line);
    get_token(t);
    exp(t);
    get_token(t);
    if(tokens.top()->type != BRACKET_CLOSE)
        error_handle(")", tokens.top()->line);
}

void return_command(tokenizer* t) {
    get_token(t);
    if(tokens.top()->type != SEMICOLON) {
        exp(t);
        get_token(t);
    }
}

void loop_command(tokenizer* t) {
    
}

// first part of this lex should be read before calling this function
void command(tokenizer* t) {
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
    else if(tokens.top()->type == VAR) {
        // decl_command, set_command, func_call
        int stack_top = tokens.size();
        if(set_command(t) == -1) {
            remove_stack_top(stack_top);
            if(decl_command(t) == -1) {
                remove_stack_top(stack_top);
                func_call(t);
            }
        }
    }
    else {
        if(tokens.top()->value == "input")
            input_command(t);
        else if(tokens.top()->value == "print")
            print_command(t);
        else if(tokens.top()->value == "break" || tokens.top()->value == "continue")
            loop_command(t);
        get_token(t);
        if(tokens.top()->type != SEMICOLON)
            error_handle(";", tokens.top()->line);
        get_token(t);    
    }
}

// first part of this lex should be read before calling this function
void commands(tokenizer* t) {
    if(tokens.top()->type == CURLY_CLOSE)
        return;
    command(t);
    commands(t);
}

// no part of this lex should be read before calling this function
void block_commands(tokenizer* t) {
    get_token(t);
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
    get_token(t);
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
    get_token(t);
    if(tokens.top()->type == KEYWORD && tokens.top()->value == "main")
        return;
    if(tokens.top()->type == KEYWORD && tokens.top()->value == "class")
        class_decl(t);
    if(tokens.top()->type == VAR)
        function(t);
}

// keyword main should be read and saved in token before calling this function
void main_function(tokenizer* t) {
    get_token(t);
    if(tokens.top()->type != BRACKET_OPEN) 
        error_handle("(", tokens.top()->line);
    function_arguments(t);
    if(tokens.top()->type != BRACKET_CLOSE) 
        error_handle(")", tokens.top()->line);
    block_commands(t);
}

// no part of this lex should be read before calling this function
void program(tokenizer* t) {
    program_parts(t);
    main_function(t);
}