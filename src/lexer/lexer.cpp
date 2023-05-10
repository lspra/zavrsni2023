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

// <var> -> var | var.var
bool var(tokenizer* t) {
    std::cout << "var"  << std::endl;
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    if(tokens.top()->type != VAR)
        return false;
    get_token(t);
    if(tokens.top()->type == DOT) {
        get_token(t);
        if(tokens.top()->type != VAR)
            return false;
        get_token(t);
    }
    return true;
}

// <Lvalue> -> <var> | <var>[<exp>]
void Lvalue(tokenizer* t) {
    std::cout << "Lvalue"  << std::endl;
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    if(!var(t))
        error_handle("Lvalue", tokens.top()->line);
    if(tokens.top()->type == SQUARE_OPEN) {
        get_token(t);
        if(!exp(t))
            error_handle("expression", tokens.top()->line);
        if(tokens.top()->type != SQUARE_CLOSE)
            error_handle("]", tokens.top()->type);
        get_token(t);
    }
}

// <A> -> <Lvalue> | (<exp>) | const | <func_call>
bool A(tokenizer* t) {
    std::cout << "A"  << std::endl;
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    if(tokens.top()->type == VAR) {
        int stack_top = tokens.size();
        if(!func_call(t)) {
            remove_stack_top(stack_top);
            Lvalue(t);
        }
    } else if(tokens.top()->type == BRACKET_OPEN) {
        get_token(t);
        if(!exp(t))
            return false;
        if(tokens.top()->type != BRACKET_CLOSE) 
            error_handle(")", tokens.top()->line);
        get_token(t);
    } else if(tokens.top()->type == INT || tokens.top()->type == FLOAT || tokens.top()->type == STRING) {
        get_token(t);
    } else {
        return false;
    }
    return true;
}

// <B> -> <A> | +<A> | -<A> | ~<A>
bool B(tokenizer* t) {
    std::cout << "B"  << std::endl;
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    if(tokens.top()->value == "+" || tokens.top()->value == "-" || tokens.top()->value == "~")
        get_token(t);
    return A(t);
}

// <C> -> <B> << <C> | <B> >> <C> | <B>
bool C(tokenizer* t) {
    std::cout << "C"  << std::endl;
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    if(!B(t))
        return false;
    if(tokens.top()->value == "<<" || tokens.top()->value == ">>") {
        get_token(t);
        return C(t);
    }
    return true;
}

// <D> -> <C> & <D> | <C>
bool D(tokenizer* t) {
    std::cout << "D"  << std::endl;
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    if(!C(t))
        return false;
    if(tokens.top()->value == "&") {
        get_token(t);
        return D(t);
    }
    return true;
}

// <E> -> <D> '|' <E> | <D> ^ <E> | <D>
bool E(tokenizer* t) {
    std::cout << "E"  << std::endl;
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    if(!D(t))
        return false;
    if(tokens.top()->value == "|" || tokens.top()->value == "^") {
        get_token(t);
        return E(t);
    }
    return true;
}

// <F> -> <E> * <F> | <E> / <F> | <E> % <F> | <E>
bool F(tokenizer* t) {
    std::cout << "F"  << std::endl;
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    if(!E(t))
        return false;
    if(tokens.top()->value == "*" || tokens.top()->value == "/" || tokens.top()->value == "%") {
        get_token(t);
        return F(t);
    }
    return true;
}

// <G> -> <F> + <G> | <F> - <G> | <F>
bool G(tokenizer* t) {
    std::cout << "G"  << std::endl;
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    if(!F(t))
        return false;
    if(tokens.top()->value == "+" || tokens.top()->value == "-") {
        get_token(t);
        return G(t);
    }
    return true;
}

// <H> -> <set_command> | <G>
bool H(tokenizer* t) {
    std::cout << "H"  << std::endl;
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    int stack_top = tokens.size();
    if(!set_command(t)) {
        remove_stack_top(stack_top);
        return G(t);
    }
    return true;
}


// <I> -> <H> == <H> | <H> != <H>| <H> < <H> | <H> <= <H> | <H> > <H> | <H> >= <H> | <H>
bool I(tokenizer* t) {
    std::cout << "I"  << std::endl;
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    if(!H(t))
        return false;
    if (tokens.top()->value == "==" || tokens.top()->value == "!=" || tokens.top()->value == "<" ||
         tokens.top()->value == "<=" || tokens.top()->value == ">" || tokens.top()->value == ">=") {
        get_token(t);
        return H(t);
    }
    return true;
}

// <J> -> not <I> | <I>
bool J(tokenizer* t) {
    std::cout << "J"  << std::endl;
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    if(tokens.top()->value == "not")
        get_token(t);
    return I(t);
}

// <K> -> <J> and <K> | <J>
bool K(tokenizer* t) {
    std::cout << "K"  << std::endl;
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    if(!J(t))
        return false;
    if(tokens.top()->value == "and") {
        get_token(t);
        return K(t);
    }
    return true;
}

// <exp> -> <K> or <exp> | <K> xor <exp> | <K>
bool exp(tokenizer* t) {
    std::cout << "exp"  << std::endl;
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    if(!K(t))
        return false;
    if(tokens.top()->value == "or" || tokens.top()->value == "xor") {
        get_token(t);
        return exp(t);
    }
    return true;
}

// <argument_list> -> <exp>, <argument_list> | <epx>
void argument_list(tokenizer* t) {
    std::cout << "argument list"  << std::endl;
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    if(!exp(t))
        error_handle("expression", tokens.top()->line);
    if(tokens.top()->type == COMMA) {
        get_token(t);
        argument_list(t);
    }
}

// <arguments> -> <argument_list> | $
void arguments(tokenizer* t) {
    std::cout << "arguments"  << std::endl;
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    if(tokens.top()->type != BRACKET_CLOSE)
        argument_list(t);
}

// <func_call> -> <var>(<arguments>)
// returns true if what the function is parsing is set_command
// error handling should be done after calling the function
bool func_call(tokenizer* t) {
    std::cout << "function call"  << std::endl;
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    if(!var(t))
        return false;
    if(tokens.top()->type != BRACKET_OPEN)
        return false;
    get_token(t);
    arguments(t);
    if(tokens.top()->type != BRACKET_CLOSE)
        return false;
    get_token(t);
    return true;
}

// <decl_command> -> <Lvalue> = data_type
bool decl_command(tokenizer* t) {
    std::cout << "decl command"  << std::endl;
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    if(tokens.top()->type != VAR)
        return false;
    get_token(t);
    if(tokens.top()->value != "=")
        return false;
    get_token(t);
    if(tokens.top()->type != DATA_TYPE)
        return false;
    get_token(t);
    return true;
}

// <set_command> -> <Lvalue> operator_modify | operator_modify <Lvalue> 
// <set_command> -> <Lvalue> operator_modify (<exp>) | (<exp>) operator_modify <Lvalue>
// <set_command> -> <Lvalue> operator_modify const | const operator_modify <Lvalue>
// <set_command> -> <Lvalue> = <exp>
// returns true if what the function is parsing is set_command
// error handling should be done after calling the function
bool set_command(tokenizer* t) {
    std::cout << "set command"  << std::endl;
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    if(tokens.top()->type == OPERATOR_MODIFY) {
        get_token(t);
        Lvalue(t);
    }
    int stack_top = tokens.size();
    if(tokens.top()->type == VAR) {
        Lvalue(t);
        if(tokens.top()->type == OPERATOR_MODIFY) {
            get_token(t);
            if(tokens.top()->type == BRACKET_OPEN) {
                get_token(t);
                if(!exp(t))
                    error_handle("expression", tokens.top()->line);
                if(tokens.top()->type != BRACKET_CLOSE)
                    error_handle(")", tokens.top()->line);
                get_token(t);
            } else if(tokens.top()->type == INT || tokens.top()->type == FLOAT) {
                get_token(t);
            }
        } else if(tokens.top()->value == "=") {
            get_token(t);
            if(!exp(t))
                return false;
        } else {
            return false;
        }
    } else if(tokens.top()->type == BRACKET_OPEN) {
        get_token(t);
        if(!exp(t))
            return false;
        if(tokens.top()->type != BRACKET_CLOSE)
            return false;
        get_token(t);
        if(tokens.top()->type == OPERATOR_MODIFY) {
            get_token(t);
            Lvalue(t);
        } else
            return false;
    } else if(tokens.top()->type == INT || tokens.top()->type == FLOAT) {
        get_token(t);
        if(tokens.top()->type == OPERATOR_MODIFY) {
            get_token(t);
            Lvalue(t);
        } else {
            return false;
        }
    } else {
        return false;
    }
    return true;
}

// <if_command> -> if(<exp>) <block_commands> | if(<exp>) <block_commands> else <block_commands>
void if_command(tokenizer* t) {
    std::cout << "if command"  << std::endl;
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    get_token(t);
    if(tokens.top()->type != BRACKET_OPEN)
        error_handle("(", tokens.top()->line);
    get_token(t);
    if(!exp(t))
        error_handle("expression", tokens.top()->line);
    if(tokens.top()->type != BRACKET_CLOSE)
        error_handle(")", tokens.top()->line);
    get_token(t);
    block_commands(t);
    if(tokens.top()->value == "else") {
        get_token(t);
        block_commands(t);
    }
}

// <for_command> -> for(<set_command>; <exp>; <set_command>) <block_commands>
// <for_command> -> for(<decl_command>; <exp>; <set_command>) <block_commands>
void for_command(tokenizer* t) {
    std::cout << "for command"  << std::endl;
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    get_token(t);
    if(tokens.top()->type != BRACKET_OPEN)
        error_handle("(", tokens.top()->line);
    get_token(t);
    int stack_top = tokens.size();
    if(!set_command(t)) {
        remove_stack_top(stack_top);
        if(!decl_command(t))
            error_handle("command", tokens.top()->line);
    }
    if(tokens.top()->type != SEMICOLON)
        error_handle(";", tokens.top()->line);
    get_token(t);
    if(!exp(t))
        error_handle("expression", tokens.top()->line);
    if(tokens.top()->type != SEMICOLON)
        error_handle(";", tokens.top()->line);
    get_token(t);
    if(!set_command(t))
        error_handle("command", tokens.top()->line);
    if(tokens.top()->type != BRACKET_CLOSE)
        error_handle(")", tokens.top()->line);
    get_token(t);
    block_commands(t);
}

// <while_command> -> while(<exp>) <block_commands>
void while_command(tokenizer* t) {
    std::cout << "while command"  << std::endl;
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    get_token(t);
    if(tokens.top()->type != BRACKET_OPEN)
        error_handle("(", tokens.top()->line);
    get_token(t);
    if(!exp(t))
        error_handle("expression", tokens.top()->line);
    get_token(t);
    if(tokens.top()->type != BRACKET_CLOSE)
        error_handle(")", tokens.top()->line);
    get_token(t);
    block_commands(t);
}

// <input_command> -> input(<Lvalue>)
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

// <print_command> -> print(<exp>)
void print_command(tokenizer* t) {
    std::cout << "print command"  << std::endl;
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    get_token(t);
    if(tokens.top()->type != BRACKET_OPEN)
        error_handle("(", tokens.top()->line);
    get_token(t);
    if(!exp(t))
        error_handle("expression", tokens.top()->line);
    if(tokens.top()->type != BRACKET_CLOSE)
        error_handle(")", tokens.top()->line);
    get_token(t);
}

// <return_command> -> return <exp> | return
void return_command(tokenizer* t) {
    std::cout << "return command"  << std::endl;
    get_token(t);
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    if(tokens.top()->type != SEMICOLON) {
        if(!exp(t))
            error_handle("expression", tokens.top()->line);
    }
}

// <loop_command> -> break | continue
void loop_command(tokenizer* t) {
    get_token(t);
}

// <command> -> <decl_command>; | <if_commmand> | <for_command> | <while_command>
// <command> -> <input_command>; | <print_command>; | <func_call>;
// <command> -> <set_command>; | <return_command>; | <loop_command>;
void command(tokenizer* t) {
    std::cout << "command"  << std::endl;
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    if(tokens.top()->value == "if")
        if_command(t);
    else if(tokens.top()->value == "for")
        for_command(t);
    else if(tokens.top()->value == "while")
        while_command(t);
    else {
        if(tokens.top()->value == "return")
            return_command(t);
        else if(tokens.top()->value == "input")
            input_command(t);
        else if(tokens.top()->value == "print")
            print_command(t);
        else if(tokens.top()->value == "break" || tokens.top()->value == "continue")
            loop_command(t);
        else if(tokens.top()->type == VAR) {
            // decl_command, set_command, func_call
            int stack_top = tokens.size();
            if(!set_command(t)) {
                remove_stack_top(stack_top);
                if(!decl_command(t)) {
                    remove_stack_top(stack_top);
                    func_call(t);
                }
            }
        }
        if(tokens.top()->type != SEMICOLON)
            error_handle(";", tokens.top()->line);
        get_token(t);    
    }
}

// <commands> -> <command> <commands> | $
void commands(tokenizer* t) {
    std::cout << "commands"  << std::endl;
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    if(tokens.top()->type == CURLY_CLOSE)
        return;
    command(t);
    commands(t);
}

// <block_commands> -> {<commands>} | <command>
void block_commands(tokenizer* t) {
    std::cout << "block commands"  << std::endl;
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    if(tokens.top()->type == CURLY_OPEN) {
        get_token(t);
        commands(t);
        if(tokens.top()->type != CURLY_CLOSE)
            error_handle("}", tokens.top()->line);
        get_token(t);
    } else {
        command(t);
    }
}

// <func_arguments_list> -> <decl_command>, <func_arguments_list> | <decl_command>
void func_arguments_list(tokenizer* t) {
    std::cout << "func arguments list"  << std::endl;
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    if(!decl_command(t))
        error_handle("declaration command", tokens.top()->line);
    if(tokens.top()->type == COMMA) {
        get_token(t);
        func_arguments_list(t);
    }
}

// <function_arguments> -> <func_arguments_list> | $
void function_arguments(tokenizer* t) {
    std::cout << "function arguments" << std::endl;
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    if(tokens.top()->type == VAR) {
        func_arguments_list(t);
    }
}

// <return_type> -> =data_type | $
void return_type(tokenizer* t) {
    std::cout << "return type"  << std::endl;
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    if(tokens.top()->value != "=")
        return;
    get_token(t);
    if(tokens.top()->type != DATA_TYPE)
        error_handle("return type", tokens.top()->line);
    get_token(t);
}

// <function> -> var (<function_arguments>) <return_type> <block_commands>
void function(tokenizer* t) {
    std::cout << "function"  << std::endl;
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    get_token(t);
    if(tokens.top()->type != BRACKET_OPEN)
        error_handle("(", tokens.top()->line);
    get_token(t);
    function_arguments(t);
    if(tokens.top()->type != BRACKET_CLOSE) 
        error_handle(")", tokens.top()->line);
    get_token(t);
    return_type(t);
    block_commands(t);
}

// <base_list> -> var, <base_list> | var
void base_list(tokenizer* t) {
    std::cout << "base list"  << std::endl;
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    if(tokens.top()->type != VAR) 
        error_handle("base class name", tokens.top()->line);
    get_token(t);
    if(tokens.top()->type == COMMA) {
        get_token(t);
        base_list(t);
    }
}

// <base_classes> -> (<base_list>) | $
void base_classes(tokenizer *t) {
    std::cout << "base classes"  << std::endl;
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

// <visibility_specifier> -> public | private
void visibility_specifier(tokenizer* t) {
    std::cout << "visibility specifier"  << std::endl;
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    if(tokens.top()->value != "private" && tokens.top()->value != "public")
        error_handle("visibility specifier", tokens.top()->line);
    get_token(t);
    if(tokens.top()->type != COLON)
        error_handle(":", tokens.top()->line);
    get_token(t);
}

// <visibility_block> -> <visibility_specifier> <program_parts>
void visibility_block(tokenizer* t) {
    std::cout << "visibility block"  << std::endl;
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    visibility_specifier(t);
    program_parts(t);
}

// <class_body> -> <visibility_block> <class_body> | <visibility_block>
void class_body(tokenizer* t) {
    std::cout << "class body"  << std::endl;
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    visibility_block(t);
    if(tokens.top()->type != CURLY_CLOSE)
        class_body(t);
}

// <class_decl> -> class var <base_classes> {<class_body>}
void class_decl(tokenizer* t) {
    std::cout << "class decl"  << std::endl;
    get_token(t);
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    if(tokens.top()->type != VAR) 
        error_handle("class name", tokens.top()->line);
    get_token(t);
    base_classes(t);
    if(tokens.top()->type != CURLY_OPEN) 
        error_handle("{", tokens.top()->line);
    get_token(t);
    class_body(t);
    if(tokens.top()->type != CURLY_CLOSE) 
        error_handle("}", tokens.top()->line);
    get_token(t);
}

// <program_parts> -> <class_decl> <program_parts> | <function> <program_parts> 
// <program_parts> -> <set_command>; <program_parts> | <decl_command>; <program_parts> | $
void program_parts(tokenizer* t) {
    std::cout << "program parts"  << std::endl;
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    if(tokens.top()->value == "class")
        class_decl(t);
    else if(tokens.top()->type == VAR) {
        int stack_top = tokens.size();
        if(set_command(t)) {
            if(tokens.top()->type != SEMICOLON)
                error_handle(";", tokens.top()->line);
            get_token(t);
        } else {
            remove_stack_top(stack_top);
            if(decl_command(t)) {
                if(tokens.top()->type != SEMICOLON)
                    error_handle(";", tokens.top()->line);
                get_token(t);
            } else {
                remove_stack_top(stack_top);
                function(t);
            }
        }
    }
    if(tokens.top()->value != "main" 
     && tokens.top()->value != "public"
     && tokens.top()->value != "private"
     && tokens.top()->type != CURLY_CLOSE)
        program_parts(t);
}

// <main_function> -> main (<function_arguments>) <block_commands>
void main_function(tokenizer* t) {
    std::cout << "main function"  << std::endl;
    std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
    get_token(t);
    if(tokens.top()->type != BRACKET_OPEN) 
        error_handle("(", tokens.top()->line);
    get_token(t);
    function_arguments(t);
    if(tokens.top()->type != BRACKET_CLOSE) 
        error_handle(")", tokens.top()->line);
    get_token(t);
    block_commands(t);
}

// <program> -> <program_parts> <main_function>
// before calling each function, the token that begins that function,
// should be on the top of the tokens stack
// after each function is executed, token that follows that function
// is on the top of the tokens stack
void program(tokenizer* t) {
    std::cout << "program"  << std::endl;
    get_token(t);
    program_parts(t);
    main_function(t);
    if(tokens.top()->type != END_OF_FILE)
        error_handle("EOF", tokens.top()->line);
}