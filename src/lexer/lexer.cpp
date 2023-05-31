#include <iostream>
#include <stack>
#include "lexer.hpp"

std::stack<Token*> tokens;
std::stack<Token*> temp_removed;
Scope* global_scope;

void remove_stack_top() {
	temp_removed.push(tokens.top());
	tokens.pop();
}

void remove_stack_top(int index) {
	for(int i = tokens.size() - index; i > 0; i--)
		remove_stack_top();
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

void error_handle(std::string expected) {
	std::cerr << "Expected " << expected << " in line " << tokens.top()->line << std::endl;
	exit(1);
}

data_types convert_to_type(Token* token) {
	if(token->type != DATA_TYPE)
		error_handle("cannot convert this");
	if(token->type == INT)
		return int32;
	if(token->type == FLOAT)
		return float32;
	if(token->type == STRING)
		return string_type;
}

bool convertible(Var_object* from, Var_object* to) {
	if(to->type == class_instance) {
		if(from->type != class_instance)
			return false;
		Class_instance* from_class = (Class_instance*) from;
		Class_instance* to_class = (Class_instance*) to;
		return from_class->class_type->base_classes.contains(to_class->class_type);
	}
	if(from->type == to->type)
		return true;
	if (to->type == bool_type)
		return true;
	if(to->type <= float96)
		return from->type != string_type;
	return from->type == string_type && to->type == string_type;
}

Variable* find_var(Token* var, std::vector<Variable*> variables) {

	for(unsigned int i = 0; i < variables.size(); i++) {
		if(variables[i]->name == var->value)
			return variables[i];
	}
	return nullptr;
}

// <var_extend> -> <var_extend>
Variable* Class::var_extend (tokenizer* t, Scope* scope) {
	if(constructor != nullptr)
		return constructor->var_extend(t, scope);
	constructor = new Function(this->name, this->class_scope, new Class_instance("", this));
	return constructor;
}

// <var_extend> -> (<arguments>) <var_extend>
Variable* Function::var_extend (tokenizer* t, Scope* scope)
{
	if (tokens.top()->type == BRACKET_OPEN) {
		get_token(t);
		arguments(t, scope);
		if(tokens.top()->type != BRACKET_CLOSE)
			error_handle(")");
	} else {
		error_handle("arguments");
	}
	get_token(t);
	return return_object;
}

// <var_extend> -> .var <var_extend>
Variable* Class_instance::var_extend (tokenizer* t, Scope* scope) {
	if (tokens.top()->type != DOT)
		return this;
	get_token(t);
	if(tokens.top()->type != VAR)
		error_handle("member name");
	scope = this->class_type->class_scope;
	while(scope != global_scope) {
		Variable *new_var = find_var(tokens.top(), scope->variables);
		if(new_var != nullptr) {
			return new_var;
		}
		scope = scope->parent_scope;
	}
	error_handle("Class has no member with name:" + tokens.top()->value);
	return nullptr;
}

// <var_extend> -> [<exp>] <var_extend>
Variable* array::var_extend (tokenizer* t, Scope* scope) {
	if(tokens.top()->type == SQUARE_OPEN) {
		get_token(t);
		exp(t, scope);
		if(tokens.top()->type != SQUARE_CLOSE)
			error_handle("]");
	} else
		return this;
	get_token(t);
	return new array(name, dimension - 1, size);
}

// <var> -> var <var_extend> <var>
// returns Variable/Function/Class pointer if this is already initialised varible/function
// returns nullptr if there is an error
// returns Variable with type -1 if varible/class/function is not initialised
Var_object* var(tokenizer* t, Scope* scope, bool* Lvalue) {
	std::cout << "var"  << std::endl;
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	if(tokens.top()->type != VAR)
		return nullptr;
	Scope* cur_scope = scope;
	Variable* variable = nullptr;
	get_token(t);
	while(scope != nullptr && variable != nullptr) {
		if(tokens.top()->type != BRACKET_OPEN)
			*Lvalue = false;
		variable = find_var(tokens.top(), cur_scope->variables);
		if(variable == nullptr)
			cur_scope = cur_scope->parent_scope;
	}
	if(variable == nullptr) {
		error_handle("undefined variable error");
	}
	Variable* new_variable;
	while(new_variable != variable) {
		variable = new_variable;
		new_variable = variable->var_extend(t, scope);
	}
	if(new_variable->type == class_ || new_variable->type == function_)
		error_handle("variable");
	return (Var_object*) new_variable;
}

Var_object* Lvalue(tokenizer* t, Scope* scope) {
	bool lvalue;
	Var_object* variable = var(t, scope, &lvalue);
	if(!lvalue || variable->type == -1)
		error_handle("defined Lvalue");
	return variable;
}

// <A> -> <Lvalue> = <exp>
// <A> -> <Lvalue> | (<exp>) | const
Var_object* A(tokenizer* t, Scope* scope) {
	std::cout << "A"  << std::endl;
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	if(tokens.top()->type == VAR) {
		Token* var_token = tokens.top();
		get_token(t);
		// special case if this is var = <exp> and var is undefined
		if (tokens.top()->value == "=") {
			Variable* variable = find_var(var_token, scope->variables);
			if (variable == nullptr) {
				exp(t, scope); // get type
				Simple_Variable* variable = new Simple_Variable(var_token->value, bool_type);
				scope->variables.push_back(variable);
				return variable;
			}
			remove_stack_top();
		}
		bool lvalue;
		Var_object* variable = var(t, scope, &lvalue);
		if (tokens.top()->value == "=") {
			if(!lvalue)
				error_handle("Lvalue");
			get_token(t);
			// TODO get type from exp and check if it is same type
			Var_object* expression = exp(t, scope);
			if(convertible(expression, variable))
				return variable;
			else
				error_handle("Cannot convert one type to another");
		}
	} 
	if(tokens.top()->type == BRACKET_OPEN) {
		get_token(t);
		Var_object* expression = exp(t, scope);
		if(tokens.top()->type != BRACKET_CLOSE)
			error_handle(")");
		get_token(t);
		return expression;
	} 
	if(tokens.top()->type == INT || tokens.top()->type == FLOAT || tokens.top()->type == STRING) 
		return new Simple_Variable("", convert_to_type(tokens.top()));
	error_handle("not okay");
}

// <B> -> <A> | +<A> | -<A> | ~<A>
Var_object* B(tokenizer* t, Scope* scope) {
	std::cout << "B"  << std::endl;
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	if(tokens.top()->value == "+" || tokens.top()->value == "-" || tokens.top()->value == "~") {
		// TODO check if operator is defined on typeof A
		get_token(t);
	}
	return A(t, scope);
}

// <C> -> <B> << <C> | <B> >> <C> | <B>
Var_object* C(tokenizer* t, Scope* scope) {
	std::cout << "C"  << std::endl;
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	Var_object* b = B(t, scope);
	if(tokens.top()->value == "<<" || tokens.top()->value == ">>") {
		get_token(t);
		Var_object* c = C(t, scope);
		// TODO check if b and c are int_type
	}
	return b;
}

// <D> -> <C> & <D> | <C>
Var_object* D(tokenizer* t, Scope* scope) {
	std::cout << "D"  << std::endl;
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	Var_object* c = C(t, scope);
	if(tokens.top()->value == "&") {
		get_token(t);
		Var_object* d = D(t, scope);
		// TODO check if c and d are okey types
	}
	return c;
}

// <E> -> <D> '|' <E> | <D> ^ <E> | <D>
Var_object* E(tokenizer* t, Scope* scope) {
	std::cout << "E"  << std::endl;
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	Var_object* d = D(t, scope);
	if(tokens.top()->value == "|" || tokens.top()->value == "^") {
		get_token(t);
		Var_object* e = E(t, scope);
		// TODO check types
	}
	return d;
}

// <F> -> <E> * <F> | <E> / <F> | <E> % <F> | <E>
Var_object* F(tokenizer* t, Scope* scope) {
	std::cout << "F"  << std::endl;
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	Var_object* e = E(t, scope);
	if(tokens.top()->value == "*" || tokens.top()->value == "/" || tokens.top()->value == "%") {
		get_token(t);
		// TODO check types
		Var_object* f = F(t, scope);
	}
	return e;
}

// <G> -> <F> + <G> | <F> - <G> | <F>
Var_object* G(tokenizer* t, Scope* scope) {
	std::cout << "G"  << std::endl;
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	Var_object* f = F(t, scope);
	if(tokens.top()->value == "+" || tokens.top()->value == "-") {
		get_token(t);
		// TODO check types
		Var_object* g = G(t, scope);
	}
	return f;
}

// <H> -> <Lvalue> operator_modify | operator_modify <Lvalue> |
//    <Lvalue> operator_modify (<exp>) | (<exp>) operator_modify <Lvalue> |
//    <Lvalue> operator_modify const | const operator_modify <Lvalue>  | <G>
Var_object* H(tokenizer* t, Scope* scope) {
	std::cout << "H"  << std::endl;
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	int stack_top = tokens.size();

	if(tokens.top()->type == OPERATOR_MODIFY) {
		get_token(t);
		Var_object* variable = Lvalue(t, scope);
		// TODO check type
		return variable;
	}
	if(tokens.top()->type == VAR) {
		bool lvalue;
		Var_object* variable = var(t, scope, &lvalue);
		if(tokens.top()->type == OPERATOR_MODIFY) {
			if(!lvalue)
				error_handle("Lvalue");
			get_token(t);
			if(tokens.top()->type == BRACKET_OPEN) {
				get_token(t);
				if(!exp(t, scope))
					error_handle("expression");
				if(tokens.top()->type != BRACKET_CLOSE)
					error_handle(")");
				get_token(t);
			} else if(tokens.top()->type == INT || tokens.top()->type == FLOAT) {
				get_token(t);
			}
			// TODO check type
			return variable;
		}
		remove_stack_top(stack_top);
		return G(t, scope);
	} 
	
	Var_object* expression;
	if(tokens.top()->type == BRACKET_OPEN) {
		get_token(t);
		// TODO check if int/float type
		expression = exp(t, scope);
		if(tokens.top()->type != BRACKET_CLOSE)
			error_handle(")");
		get_token(t);
	} else if(tokens.top()->type == INT || tokens.top()->type == FLOAT) {
		expression = new Simple_Variable("", convert_to_type(tokens.top()));
		get_token(t);
	}

	if(tokens.top()->type != OPERATOR_MODIFY) {
		remove_stack_top(stack_top);
		return G(t, scope);
	}
	get_token(t);
	// TODO check types
	Var_object* variable = Lvalue(t, scope);
	return variable;
}


// <I> -> <H> == <H> | <H> != <H>| <H> < <H> | <H> <= <H> | <H> > <H> | <H> >= <H> | <H>
Var_object* I(tokenizer* t, Scope* scope) {
	std::cout << "I"  << std::endl;
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	Var_object* h1 = H(t, scope);
	if (tokens.top()->value == "==" || tokens.top()->value == "!=" || tokens.top()->value == "<" ||
		 tokens.top()->value == "<=" || tokens.top()->value == ">" || tokens.top()->value == ">=") {
		get_token(t);
		Var_object* h2 = H(t, scope);
	}
	// TODO return type of bool
	return h1;
}

// <J> -> not <I> | <I>
Var_object* J(tokenizer* t, Scope* scope) {
	std::cout << "J"  << std::endl;
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	if(tokens.top()->value == "not")
		get_token(t);
	return I(t, scope);
}

// <K> -> <J> and <K> | <J>
Var_object* K(tokenizer* t, Scope* scope) {
	std::cout << "K"  << std::endl;
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	Var_object* j = J(t, scope);
	if(tokens.top()->value == "and") {
		get_token(t);
		Var_object* k = K(t, scope);
	}
	return j;
}

// <exp> -> <K> or <exp> | <K> xor <exp> | <K>
Var_object* exp(tokenizer* t, Scope* scope) {
	std::cout << "exp"  << std::endl;
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	Var_object* k =	K(t, scope);
	if(tokens.top()->value == "or" || tokens.top()->value == "xor") {
		get_token(t);
		Var_object* expresssion = exp(t, scope);
	}
	return k;
}

// <argument_list> -> <exp>, <argument_list> | <epx>
void argument_list(tokenizer* t, Scope* scope) {
	std::cout << "argument list"  << std::endl;
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	if(!exp(t, scope))
		error_handle("expression");
	if(tokens.top()->type == COMMA) {
		get_token(t);
		argument_list(t, scope);
	}
}

// <arguments> -> <argument_list> | $
// TODO add checking if this are correct argument types and amount
void arguments(tokenizer* t, Scope* scope) {
	std::cout << "arguments"  << std::endl;
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	if(tokens.top()->type != BRACKET_CLOSE)
		argument_list(t, scope);
}

// <decl_command> -> var = data_type
bool decl_command(tokenizer* t, Scope* scope) {
	std::cout << "decl command"  << std::endl;
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	if(tokens.top()->type != VAR)
		return false;
	Token* var = tokens.top();
	get_token(t);
	if(tokens.top()->value != "=")
		return false;
	get_token(t);
	// TODO ovo moze biti i class
	if(tokens.top()->type != DATA_TYPE)
		return false;
	Simple_Variable* variable = new Simple_Variable(var->value, bool_type);
	// var.type = data_types.index(tokens.top()->value);
	scope->variables.push_back(variable);
	get_token(t);
	return true;
}

// <if_command> -> if(<exp>) <block_commands> | if(<exp>) <block_commands> else <block_commands>
void if_command(tokenizer* t, Scope* scope) {
	std::cout << "if command"  << std::endl;
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	get_token(t);
	Scope* if_scope = new Scope(scope);
	if(tokens.top()->type != BRACKET_OPEN)
		error_handle("(");
	get_token(t);
	if(!exp(t, if_scope))
		error_handle("expression");
	if(tokens.top()->type != BRACKET_CLOSE)
		error_handle(")");
	get_token(t);
	block_commands(t, if_scope);
	if(tokens.top()->value == "else") {
		get_token(t);
		block_commands(t, if_scope);
	}
}

// <for_command> -> for(<exp>; <exp>; <exp>) <block_commands>
// <for_command> -> for(<decl_command>; <exp>; <exp>) <block_commands>
void for_command(tokenizer* t, Scope* scope) {
	std::cout << "for command"  << std::endl;
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	Scope* for_scope = new Scope(scope);
	for_scope->parent_scope = scope;
	get_token(t);
	if(tokens.top()->type != BRACKET_OPEN)
		error_handle("(");
	get_token(t);
	int stack_top = tokens.size();
	if(!decl_command(t, for_scope)) {
		remove_stack_top(stack_top);
		if(!exp(t, for_scope))
			error_handle("command");
	}
	if(tokens.top()->type != SEMICOLON)
		error_handle(";");
	get_token(t);
	if(!exp(t, for_scope))
		error_handle("expression");
	if(tokens.top()->type != SEMICOLON)
		error_handle(";");
	get_token(t);
	if(!exp(t, for_scope))
		error_handle("command");
	if(tokens.top()->type != BRACKET_CLOSE)
		error_handle(")");
	get_token(t);
	block_commands(t, for_scope);
}

// <while_command> -> while(<exp>) <block_commands>
void while_command(tokenizer* t, Scope* scope) {
	std::cout << "while command"  << std::endl;
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	Scope* while_scope = new Scope(scope);
	get_token(t);
	if(tokens.top()->type != BRACKET_OPEN)
		error_handle("(");
	get_token(t);
	if(!exp(t, while_scope))
		error_handle("expression");
	get_token(t);
	if(tokens.top()->type != BRACKET_CLOSE)
		error_handle(")");
	get_token(t);
	block_commands(t, while_scope);
}

// <input_command> -> input(<Lvalue>)
void input_command(tokenizer* t, Scope* scope) {
	std::cout << "input command"  << std::endl;
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	get_token(t);
	if(tokens.top()->type != BRACKET_OPEN)
		error_handle("(");
	get_token(t);
	Variable* variable = Lvalue(t, scope);
	if(tokens.top()->type != BRACKET_CLOSE)
		error_handle(")");
	get_token(t);
}

// <print_command> -> print(<exp>)
void print_command(tokenizer* t, Scope* scope) {
	std::cout << "print command"  << std::endl;
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	get_token(t);
	if(tokens.top()->type != BRACKET_OPEN)
		error_handle("(");
	get_token(t);
	if(!exp(t, scope))
		error_handle("expression");
	if(tokens.top()->type != BRACKET_CLOSE)
		error_handle(")");
	get_token(t);
}

// <return_command> -> return <exp> | return
void return_command(tokenizer* t, Scope* scope) {
	std::cout << "return command"  << std::endl;
	get_token(t);
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	if(tokens.top()->type != SEMICOLON) {
		if(!exp(t, scope))
			error_handle("expression");
	}
}

// <loop_command> -> break | continue
void loop_command(tokenizer* t) {
	get_token(t);
}

// <command> -> <decl_command>; | <if_commmand> | <for_command> | <while_command>
// <command> -> <input_command>; | <print_command>;
// <command> -> <exp>; | <return_command>; | <loop_command>;
void command(tokenizer* t, Scope* scope) {
	std::cout << "command"  << std::endl;
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	if(tokens.top()->value == "if")
		if_command(t, scope);
	else if(tokens.top()->value == "for")
		for_command(t, scope);
	else if(tokens.top()->value == "while")
		while_command(t, scope);
	else {
		if(tokens.top()->value == "return")
			return_command(t, scope);
		else if(tokens.top()->value == "input")
			input_command(t, scope);
		else if(tokens.top()->value == "print")
			print_command(t, scope);
		else if(tokens.top()->value == "break" || tokens.top()->value == "continue")
			loop_command(t);
		else if(tokens.top()->type == VAR) {
			int stack_top = tokens.size();
			if(!decl_command(t, scope)) 
				remove_stack_top(stack_top);
		}
		if(tokens.top()->type != SEMICOLON)
			error_handle("command");
		get_token(t);
	}
}

// <commands> -> <command> <commands> | $
void commands(tokenizer* t, Scope* scope) {
	std::cout << "commands"  << std::endl;
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	if(tokens.top()->type == CURLY_CLOSE)
		return;
	command(t, scope);
	commands(t, scope);
}

// <block_commands> -> {<commands>} | <command>
void block_commands(tokenizer* t, Scope* scope) {
	std::cout << "block commands"  << std::endl;
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	if(tokens.top()->type == CURLY_OPEN) {
		get_token(t);
		commands(t, scope);
		if(tokens.top()->type != CURLY_CLOSE)
			error_handle("}");
		get_token(t);
	} else {
		command(t, scope);
	}
}

// <func_arguments_list> -> <decl_command>, <func_arguments_list> | <decl_command>
void func_arguments_list(tokenizer* t, Function* function) {
	std::cout << "func arguments list"  << std::endl;
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	// TODO check if this function already has this argument
	if(!decl_command(t, function->function_scope))
		error_handle("argument");
	function->function_parameters.push_back(function->function_scope->variables.back());
	if(tokens.top()->type == COMMA) {
		get_token(t);
		func_arguments_list(t, function);
	}
}

// <function_arguments> -> <func_arguments_list> | $
void function_arguments(tokenizer* t, Function* function) {
	std::cout << "function arguments" << std::endl;
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	if(tokens.top()->type == VAR) {
		func_arguments_list(t, function);
	}
}

// <return_type> -> =data_type | $
void return_type(tokenizer* t, Function* function) {
	std::cout << "return type"  << std::endl;
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	if(tokens.top()->value != "=")
		return;
	get_token(t);
	if(tokens.top()->type != DATA_TYPE)
		error_handle("return type");
	function->return_object = new Simple_Variable("", bool_type);
	// function.type = data_types.index(tokens.top()->value);
	get_token(t);
}

// <function> -> var (<function_arguments>) <return_type> <block_commands>
void function(tokenizer* t, Scope* scope) {
	std::cout << "function"  << std::endl;
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	get_token(t);
	Function* function = new Function(tokens.top()->value, scope);
	if(tokens.top()->type != BRACKET_OPEN)
		error_handle("(");
	get_token(t);
	function_arguments(t, function);
	if(tokens.top()->type != BRACKET_CLOSE)
		error_handle(")");
	get_token(t);
	return_type(t, function);
	block_commands(t, function->function_scope);
	// TODO check if function with this name and arguments already exists
	scope->variables.push_back(function);
}

// <base_list> -> var, <base_list> | var
void base_list(tokenizer* t, Scope* scope) {
	std::cout << "base list"  << std::endl;
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	if(tokens.top()->type != VAR)
		error_handle("base class name");
	get_token(t);
	if(tokens.top()->type == COMMA) {
		get_token(t);
		base_list(t, scope);
	}
}

// <base_classes> -> (<base_list>) | $
void base_classes(tokenizer *t, Scope* scope) {
	std::cout << "base classes"  << std::endl;
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	if(tokens.top()->type == CURLY_OPEN)
		return;
	if(tokens.top()->type != BRACKET_OPEN)
		error_handle("(");
	base_list(t, scope);
	if(tokens.top()->type != BRACKET_CLOSE)
		error_handle(")");
	get_token(t);
}

// <visibility_specifier> -> public | private
void visibility_specifier(tokenizer* t) {
	std::cout << "visibility specifier"  << std::endl;
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	if(tokens.top()->value != "private" && tokens.top()->value != "public")
		error_handle("visibility specifier");
	get_token(t);
	if(tokens.top()->type != COLON)
		error_handle(":");
	get_token(t);
}

// <visibility_block> -> <visibility_specifier> <program_parts>
void visibility_block(tokenizer* t, Scope* scope) {
	std::cout << "visibility block"  << std::endl;
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	visibility_specifier(t);
	program_parts(t, scope);
}

// <class_body> -> <visibility_block> <class_body> | <visibility_block>
void class_body(tokenizer* t, Scope* scope) {
	std::cout << "class body"  << std::endl;
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	visibility_block(t, scope);
	if(tokens.top()->type != CURLY_CLOSE)
		class_body(t, scope);
}

// <class_decl> -> class var <base_classes> {<class_body>}
Class class_decl(tokenizer* t, Scope* scope) {
	std::cout << "class decl"  << std::endl;
	get_token(t);
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	if(tokens.top()->type != VAR)
		error_handle("class name");
	Class class_(tokens.top()->value, scope);
	get_token(t);
	base_classes(t, class_.class_scope);
	if(tokens.top()->type != CURLY_OPEN)
		error_handle("{");
	get_token(t);
	class_body(t, class_.class_scope);
	if(tokens.top()->type != CURLY_CLOSE)
		error_handle("}");
	get_token(t);
	return class_;
}

// <program_parts> -> <class_decl> <program_parts> | <function> <program_parts>
// <program_parts> -> <decl_command>; <program_parts> | $
void program_parts(tokenizer* t, Scope* scope) {
	std::cout << "program parts"  << std::endl;
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	if(tokens.top()->value == "class")
		class_decl(t, scope);
	else if(tokens.top()->type == VAR) {
		int stack_top = tokens.size();
		if(decl_command(t, scope)) {
			if(tokens.top()->type != SEMICOLON)
				error_handle(";");
			get_token(t);
		} else {
			remove_stack_top(stack_top);
			function(t, scope);
		}
	}
	if(tokens.top()->value != "main"
	 && tokens.top()->value != "public"
	 && tokens.top()->value != "private"
	 && tokens.top()->type != CURLY_CLOSE)
		program_parts(t, scope);
}

// <main_function> -> main (<function_arguments>) <block_commands>
void main_function(tokenizer* t, Scope* scope) {
	std::cout << "main function"  << std::endl;
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	Function* function = new Function(tokens.top()->value, scope);
	get_token(t);
	if(tokens.top()->type != BRACKET_OPEN)
		error_handle("(");
	get_token(t);
	function_arguments(t, function);
	if(tokens.top()->type != BRACKET_CLOSE)
		error_handle(")");
	get_token(t);
	block_commands(t, function->function_scope);
}

// <program> -> <program_parts> <main_function>
// before calling each function, the token that begins that function,
// should be on the top of the tokens stack
// after each function is executed, token that follows that function
// is on the top of the tokens stack
void program(tokenizer* t) {
	std::cout << "program"  << std::endl;
	global_scope = new Scope(nullptr);
	get_token(t);
	program_parts(t, global_scope);
	main_function(t, global_scope);
	if(tokens.top()->type != END_OF_FILE)
		error_handle("EOF");
}