#include <iostream>
#include <stack>
#include "lexer.hpp"

std::stack<Token*> tokens;
std::stack<Token*> temp_removed;

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

bool isint(data_types type) {
	return type >= 0 && type <= 8;
}

bool isint(Var_object* v) {
	if(v->type != array_element)
		return isint(v->type);
	Array_element* ar_el = (Array_element*) v;
	return isint(ar_el->array_->containing_type) && ar_el->is_one_element();
}

bool isnumber(data_types type) {
	return type >= 0 && type <= 11;
}

bool isnumber(Var_object* v) {
	if(v->type != array_element)
		return isnumber(v->type);
	Array_element* ar_el = (Array_element*) v;
	return isnumber(ar_el->array_->containing_type) && ar_el->is_one_element();
}

void Array_element::add_begin(Var_object* begin) {
	if(begin_index.size() + 1 > array_->size.size())
		error_handle("index out of bound");
	if(!isint(begin))
		error_handle("index not integer");
	begin_index.push_back(begin);
}

void Array_element::add_end(Var_object* end) {
	if(end_index.size() + 1 > array_->size.size())
		error_handle("index out of bound");
	if(!isint(end))
		error_handle("index not integer");
	if(end != begin_index.back())
		error_handle("TODO intervals not supported yet");
	generated_name += "[" + end->generated_name + "]";
	end_index.push_back(end);
}

Var_object* convert(Var_object* var1, Var_object* var2) {
	if(var1->type == class_instance || var2->type == class_instance)
		error_handle("operation between classes not yet podrzane"); // TODO operations between classes
	if(var1->type == array_element) {
		Array_element* var1_array = (Array_element*) var1;
		if(!var1_array->is_one_element())
			error_handle("operations between arrays not yet podrzane"); // TODO operations between arrays
		return convert(var1_array->array_->containing_type, var2);
	}
	if(var2->type == array_element) {
		Array_element* var2_array = (Array_element*) var2;
		if(!var2_array->is_one_element())
			error_handle("operations between arrays not yet podrzane"); // TODO operations between arrays
		return convert(var1, var2_array->array_->containing_type);
	}
	if(var1->type == string_type || var2->type == string_type)
		error_handle("operations between strings not yet podrzane"); // TODO operations between arrays
	return new Var_object("", std::max(var1->type, var2->type));
}

data_types get_type_from_decl(Token* token) {
	if(token->value == "bool")
		return bool_type;
	if(token->value == "int8")
		return int8;
	if(token->value == "int16")
		return int16;
	if(token->value == "int32")
		return int32;
	if(token->value == "int64")
		return int64;
	if(token->value == "uint8")
		return uint8;
	if(token->value == "uint16")
		return uint16;
	if(token->value == "uint32")
		return uint32;
	if(token->value == "uint64")
		return uint64;
	if(token->value == "float32")
		return float32;
	if(token->value == "float64")
		return float64;
	if(token->value == "float96")
		return float96;
	if(token->value == "string")
		return string_type;
	// this shouldn't be possible
	return undefined;
}

data_types convert_to_type(Token* token) {
	if(token->type == INT)
		return int32;
	if(token->type == FLOAT)
		return float32;
	if(token->type == STRING)
		return string_type;
	error_handle("cannot convert this");
	return undefined;
}

bool convertible(Var_object* from, Var_object* to) {
	if(to->type == class_instance) {
		if(from->type != class_instance)
			return false;
		return from->class_type->base_classes.contains(to->class_type);
	}
	if(to->type == array_element) {
		Array_element* to_array = (Array_element*) to;
		if(to_array->is_one_element())
			return convertible(from, to_array->array_->containing_type);
		// TODO this could still work with many checks
		return false;
	}
	if(from->type == array_element) {
		Array_element* from_array = (Array_element*) from;
		if(from_array->is_one_element())
			return convertible(from_array->array_->containing_type, to);
		// TODO this could still work with many checks
		return false;
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

Variable* find_var_all_scopes(Scope* cur_scope) {
	Variable* variable = nullptr;
	while(cur_scope != nullptr && variable == nullptr) {
		variable = find_var(tokens.top(), cur_scope->variables);
		if(variable == nullptr) 
			cur_scope = cur_scope->parent_scope;
	}
	if(variable == nullptr) 
		return new Var_object(tokens.top()->value, undefined);
	return variable;
}

Variable* lexer::var_extend (Variable* var, Scope* scope) {
	switch(var->type) {
		case class_:
			return var_extend((Class*) var, scope);
		case function_:
			return var_extend((Function*) var, scope);
		case array_:
			return var_extend((Array*) var, scope);
		case array_element:
			return var_extend((Array_element*) var, scope);
		default:
			return var_extend((Var_object*) var, scope);
	}
}

// <var_extend> -> <var_extend>
Variable* lexer::var_extend (Class* var, Scope* scope) {
	if(var->constructor != nullptr)
		return var_extend(var->constructor, scope);
	Var_object* cl_instance = new Var_object("", class_instance);
	cl_instance->class_type = var;
	var->constructor = new Function(var->name, var->class_scope, cl_instance);
	return var->constructor;
}

// <var_extend> -> (<arguments>) <var_extend>
Variable* lexer::var_extend (Function* var, Scope* scope)
{
	if (tokens.top()->type == BRACKET_OPEN) {
		get_token(t);
		arguments(scope, &var->function_parameters);
		if(tokens.top()->type != BRACKET_CLOSE)
			error_handle(")");
	} else {
		error_handle("arguments");
	}
	get_token(t);
	return var->return_object;
}

// <var_extend> -> .var <var_extend>
Variable* lexer::var_extend (Var_object* var, Scope* scope) {
	if(var->type != class_instance)
		return var;
	if (tokens.top()->type != DOT)
		return var;
	get_token(t);
	if(tokens.top()->type != VAR)
		error_handle("member name");
	scope = var->class_type->class_scope;
	while(scope != global_scope) {
		Variable *new_var = find_var(tokens.top(), scope->variables);
		if(new_var != nullptr) {
			get_token(t);
			return new_var;
		}
		scope = scope->parent_scope;
	}
	error_handle("Class has no member with name:" + tokens.top()->value);
	return nullptr;
}

Variable* lexer::var_extend (Array* var, Scope* scope) {
	return var_extend(new Array_element(var), scope);
}

// <var_extend> -> [<exp>] <var_extend> || [<exp>:<exp>] <var_extend> || [:<exp>] <var_extend> || [<exp>:] <var_extend> || [:] <var_extend>
Variable* lexer::var_extend(Array_element* var, Scope* scope) {
	if(tokens.top()->type != SQUARE_OPEN)
		return var;
	Array_element* ar_el = new Array_element(var);
	while(tokens.top()->type == SQUARE_OPEN) {
		get_token(t);
		Var_object* begin;
		Var_object* end;
		if(tokens.top()->type == COLON)
			begin = nullptr;
		else
			begin = exp(scope);
		
		if(tokens.top()->type == SQUARE_CLOSE)
			end = begin;
		else if(tokens.top()->type == COLON) {
			get_token(t);
			if(tokens.top()->type == SQUARE_CLOSE)
				end = nullptr;
			else
				end = exp(scope);
		}
		if(tokens.top()->type != SQUARE_CLOSE)
			error_handle("]");
		get_token(t);

		ar_el->add_begin(begin);
		ar_el->add_end(end);
	}
	return ar_el;
}

// <var> -> var <var_extend> <var>
// returns Variable/Function/Class pointer if this is already initialised varible/function
// returns nullptr if varible/class/function is not initialised
Var_object* lexer::var(Scope* scope, bool* Lvalue) {
	std::cout << "var"  << std::endl;
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	if(tokens.top()->type != VAR)
		error_handle("variable");
	Variable * variable = find_var_all_scopes(scope);
	if(variable->type == undefined)
		return (Var_object*)variable;
	get_token(t);
	Variable* new_variable = variable;
	do {
		if(tokens.top()->type == BRACKET_OPEN)
			*Lvalue = false;
		variable = new_variable;
		new_variable = var_extend(variable, scope);
	} while(new_variable != nullptr && new_variable != variable);
	if(new_variable == nullptr || (new_variable->type == class_ || new_variable->type == function_))
		error_handle("variable");
	return (Var_object*) new_variable;
}

Var_object* lexer::Lvalue(Scope* scope) {
	bool lvalue = true;
	Var_object* variable = var(scope, &lvalue);
	if(!lvalue || variable == nullptr)
		error_handle("defined Lvalue");
	return variable;
}

// <A> -> <Lvalue> = <exp>
// <A> -> <Lvalue> | (<exp>) | const
Var_object* lexer::A(Scope* scope) {
	std::cout << "A"  << std::endl;
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	if(tokens.top()->type == VAR) {
		bool lvalue = true;
		Var_object* variable = var(scope, &lvalue);
		if (tokens.top()->value == "=") {
			if(!lvalue)
				error_handle("Lvalue");
			get_token(t);
			Var_object* expression = exp(scope);
			if(!convertible(expression, variable))
				error_handle("Cannot convert one type to another");
			g->generate_exp(variable, expression);
		}
		return variable;
	}
	if(tokens.top()->type == BRACKET_OPEN) {
		get_token(t);
		Var_object* expression = exp(scope);
		if(tokens.top()->type != BRACKET_CLOSE)
			error_handle(")");
		get_token(t);
		return expression;
	}
	if(tokens.top()->type == INT || tokens.top()->type == FLOAT || tokens.top()->type == STRING) {
		Token* value = tokens.top();
		get_token(t);
		Var_object* new_var = new Var_object("", convert_to_type(value));
		g->generate_exp(new_var, value);
		return new_var;
	}
	error_handle("not okay");
	return nullptr;
}

// <B> -> <A> | +<A> | -<A> | ~<A>
Var_object* lexer::B(Scope* scope) {
	std::cout << "B"  << std::endl;
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	Token* operator_ = nullptr;
	if(tokens.top()->value == "+" || tokens.top()->value == "-" || tokens.top()->value == "~") {
		operator_ = tokens.top();
		get_token(t);
	}
	Var_object* a = A(scope);
	if(operator_ != nullptr) {
		if(!isnumber(a) || (operator_->value == "~" && !isint(a)))
			error_handle("operator not defined on not numbers");
		Var_object* b = new Var_object("", a->type);
		g->generate_exp(a, b, operator_);
		return b;
	}
	return a;
}

// <C> -> <B> << <C> | <B> >> <C> | <B>
Var_object* lexer::C(Scope* scope) {
	std::cout << "C"  << std::endl;
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	Var_object* b = B(scope);
	if(tokens.top()->value == "<<" || tokens.top()->value == ">>") {
		Token* op = tokens.top();
		get_token(t);
		Var_object* b2 = B(scope);
		if(!isint(b) || !isint(b2))
			error_handle("operator not defined on not integers");
		Var_object* rez = convert(b, b2);
		g->generate_exp(b, b2, rez, op);
		b = rez;
	}
	return b;
}

// <D> -> <C> & <D> | <C>
Var_object* lexer::D(Scope* scope) {
	std::cout << "D"  << std::endl;
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	Var_object* c = C(scope);
	while (tokens.top()->value == "&") {
		Token* op = tokens.top();
		get_token(t);
		Var_object* c2 = C(scope);
		if(!isint(c) || !isint(c2))
			error_handle("operator not defined on not integers");
		Var_object* rez = convert(c, c2);
		g->generate_exp(c, c2, rez, op);
		c = rez;
	}
	return c;
}

// <E> -> <D> '|' <E> | <D> ^ <E> | <D>
Var_object* lexer::E(Scope* scope) {
	std::cout << "E"  << std::endl;
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	Var_object* d = D(scope);
	while (tokens.top()->value == "|" || tokens.top()->value == "^") {
		Token* op = tokens.top();
		get_token(t);
		Var_object* d2 = D(scope);
		if(!isint(d) || !isint(d2))
			error_handle("operator not defined on not integers");
		Var_object* rez = convert(d, d2);
		g->generate_exp(d, d2, rez, op);
		d = rez;
	}
	return d;
}

// <F> -> <E> * <F> | <E> / <F> | <E> % <F> | <E>
Var_object* lexer::F(Scope* scope) {
	std::cout << "F"  << std::endl;
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	Var_object* e = E(scope);
	while (tokens.top()->value == "*" || tokens.top()->value == "/" || tokens.top()->value == "%") {
		Token* op = tokens.top();
		get_token(t);
		Var_object* e2 = E(scope);
		if(!isnumber(e) || !isnumber(e2))
			error_handle("operator not defined on not integers");
		Var_object* rez = convert(e, e2);
		g->generate_exp(e, e2, rez, op);
		e = rez;
	}
	return e;
}

// <G> -> <F> + <G> | <F> - <G> | <F>
Var_object* lexer::G(Scope* scope) {
	std::cout << "G"  << std::endl;
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	Var_object* f = F(scope);
	while (tokens.top()->value == "+" || tokens.top()->value == "-") {
		Token* op = tokens.top();
		get_token(t);
		Var_object* f2 = F(scope);
		if(!isnumber(f) || !isnumber(f2))
			error_handle("operator not defined on not numbers");
		Var_object* sum = convert(f, f2);
		g->generate_exp(f, f2, sum, op);
		f = sum;
	}
	return f;
}

// <H> -> <Lvalue> operator_modify | operator_modify <Lvalue> |
//    <Lvalue> operator_modify (<exp>) | (<exp>) operator_modify <Lvalue> |
//    <Lvalue> operator_modify const | const operator_modify <Lvalue>  | <G>
Var_object* lexer::H(Scope* scope) {
	std::cout << "H"  << std::endl;
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	int stack_top = tokens.size();
	if(tokens.top()->type == OPERATOR_MODIFY) {
		Token* op = tokens.top();
		get_token(t);
		Var_object* variable = Lvalue(scope);
		if(!isnumber(variable))
			error_handle("operator not defined on not numbers.");
		g->generateH(variable, op);
		return variable;
	}
	if(tokens.top()->type == VAR) {
		bool lvalue = true;
		Var_object* variable = var(scope, &lvalue);
		if(tokens.top()->type == OPERATOR_MODIFY) {
			Token* op = tokens.top();
			if(variable == nullptr || !lvalue)
				error_handle("defined Lvalue");
			get_token(t);
			Var_object* expression;
			if(tokens.top()->type == BRACKET_OPEN) {
				get_token(t);
				expression = exp(scope);
				if(!isnumber(expression))
					error_handle("number");
				if(tokens.top()->type != BRACKET_CLOSE)
					error_handle(")");
				get_token(t);
				H1_args args = {.var = variable, .t = op, .exp = expression};
				H1_arguments.push(args);
			} else if(tokens.top()->type == INT || tokens.top()->type == FLOAT) {
				expression = new Var_object("", convert_to_type(tokens.top()));
				get_token(t);
				H1_args args = {.var = variable, .t = op, .exp = expression};
				H1_arguments.push(args);
			} else {
				H0_args args = {.var = variable, .t = op};
				H0_arguments.push(args);
			}
			if(!isnumber(variable))
				error_handle("operator not defined on not numbers.");
			return variable;
		}
		remove_stack_top(stack_top);
		return G(scope);
	} 
	
	Var_object* expression;
	if(tokens.top()->type == BRACKET_OPEN) {
		get_token(t);
		expression = exp(scope);
		if(tokens.top()->type != BRACKET_CLOSE)
			error_handle(")");
		get_token(t);
	} else if(tokens.top()->type == INT || tokens.top()->type == FLOAT) {
		expression = new Var_object("", convert_to_type(tokens.top()));
		get_token(t);
	} else {
		return G(scope);
	}

	if(tokens.top()->type != OPERATOR_MODIFY) {
		remove_stack_top(stack_top);
		return G(scope);
	}
	Token* op_ = tokens.top();
	get_token(t);
	Var_object* variable = Lvalue(scope);
	if(!isnumber(variable))
		error_handle("operator not defined on not numbers.");
	g->generateH(variable, op_, expression);
	return variable;
}

// <I> -> <H> == <H> | <H> != <H>| <H> < <H> | <H> <= <H> | <H> > <H> | <H> >= <H> | <H>
Var_object* lexer::I(Scope* scope) {
	std::cout << "I"  << std::endl;
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	Var_object* h1 = H(scope);
	if (tokens.top()->value == "==" || tokens.top()->value == "!=" || tokens.top()->value == "<" ||
		 tokens.top()->value == "<=" || tokens.top()->value == ">" || tokens.top()->value == ">=") {
		Token* op = tokens.top();
		get_token(t);
		Var_object* h2 = H(scope);
		Var_object* ret = new Var_object("", bool_type);
		g->generate_exp(h1, h2, ret, op);
		return ret;
	}
	return h1;
}

// <J> -> not <I> | <I>
Var_object* lexer::J(Scope* scope) {
	std::cout << "J"  << std::endl;
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	if(tokens.top()->value == "not") {
		Token* op = tokens.top();
		get_token(t);
		Var_object* i = I(scope);
		Var_object* ret = new Var_object("", bool_type);
		g->generate_exp(i, ret, op);
		return ret;
	}
	return I(scope);
}

// <K> -> <J> and <K> | <J>
Var_object* lexer::K(Scope* scope) {
	std::cout << "K"  << std::endl;
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	Var_object* j = J(scope);
	while (tokens.top()->value == "and") {
		Token* op = tokens.top();
		get_token(t);
		Var_object* j2 = J(scope);
		Var_object* rez = new Var_object("", bool_type);
		g->generate_exp(j, j2, rez, op);
		j = rez;
	}
	return j;
}

// <exp> -> <K> or <exp> | <K> xor <exp> | <K>
Var_object* lexer::exp(Scope* scope) {
	std::cout << "exp"  << std::endl;
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	Var_object* k =	K(scope);
	while (tokens.top()->value == "or" || tokens.top()->value == "xor") {
		Token* op = tokens.top();
		get_token(t);
		Var_object* k2 = K(scope);
		Var_object* rez = new Var_object("", bool_type);
		g->generate_exp(k, k2, rez, op);
		k = rez;
	}

	// generating code for expressions such as x++ found in this exp
	while (!H0_arguments.empty())
	{
		H0_args args = H0_arguments.top();
		g->generateH(args.var, args.t);
		H0_arguments.pop();
	}
	while (!H1_arguments.empty())
	{
		H1_args args = H1_arguments.top();
		g->generateH(args.var, args.t, args.exp);
		H1_arguments.pop();
	}
	return k;
}

// <argument_list> -> <exp>, <argument_list> | <epx>
void lexer::argument_list(Scope* scope, std::vector<Var_object*> *arguments_vector, size_t index) {
	std::cout << "argument list"  << std::endl;
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	if(!convertible(exp(scope), (*arguments_vector)[index]))
		error_handle("wrong argument type");
	if(tokens.top()->type == COMMA) {
		get_token(t);
		if(index + 1 == arguments_vector->size())
			error_handle("too many arguments");
		argument_list(scope, arguments_vector, index + 1);
	} else if(index + 1 != arguments_vector->size())
		error_handle("too few arguments");
}

// <arguments> -> <argument_list> | $
void lexer::arguments(Scope* scope, std::vector<Var_object*> *arguments_vector) {
	std::cout << "arguments"  << std::endl;
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	if(tokens.top()->type != BRACKET_CLOSE)
		argument_list(scope, arguments_vector, 0);
}

// <decl_command> -> var = data_type
bool lexer::decl_command(Scope* scope) {
	std::cout << "decl command"  << std::endl;
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	if(tokens.top()->type != VAR)
		return false;
	if(find_var(tokens.top(), scope->variables) != nullptr)
		return false;
	Variable* var = find_var_all_scopes(scope);
	get_token(t);
	if(tokens.top()->value != "=")
		return false;
	get_token(t);
	if(tokens.top()->type != DATA_TYPE && var->type != undefined)
		return false;
	Var_object* variable;
	if(tokens.top()->type == DATA_TYPE) {
		variable = new Var_object(var->name, get_type_from_decl(tokens.top()));
		get_token(t);
		if(tokens.top()->type != SQUARE_OPEN) {
			scope->variables.push_back(variable);
			g->generate_var(variable);
			return true;
		}
		std::vector<Var_object*> sizes;
		while (tokens.top()->type == SQUARE_OPEN) {
			get_token(t);
			sizes.push_back(exp(scope));
			if(tokens.top()->type != SQUARE_CLOSE)
				error_handle("]");
			get_token(t);
		}
		Array* arr = new Array(var->name, sizes, variable);
		g->generate_var(arr);
		scope->variables.push_back(arr);
		return true;
	}
	// var = <exp>
	Var_object* expression = exp(scope);
	variable = (Var_object*) var;
	variable->type = expression->type;
	if(expression->type == class_instance)
		variable->class_type = expression->class_type;
	if(expression->type == array_element) {
		Array_element* ar_el = (Array_element*) expression;
		if(!ar_el->is_one_element())
			error_handle("cannot copy array elements");
		variable->type = ar_el->array_->containing_type->type;
		if(variable->type == class_instance)
			variable->class_type = ar_el->array_->containing_type->class_type;
	}
	g->generate_undefined_exp(variable, expression);
	scope->variables.push_back(variable);
	return true;
}

// <if_command> -> if(<exp>) <block_commands> | if(<exp>) <block_commands> else <block_commands>
void lexer::if_command(Scope* scope) {
	std::cout << "if command"  << std::endl;
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	get_token(t);
	Scope* if_scope = new Scope(scope);
	if(tokens.top()->type != BRACKET_OPEN)
		error_handle("(");
	get_token(t);
	exp(if_scope);
	if(tokens.top()->type != BRACKET_CLOSE)
		error_handle(")");
	get_token(t);
	block_commands(if_scope);
	if(tokens.top()->value == "else") {
		get_token(t);
		block_commands(if_scope);
	}
}

// <for_command> -> for(<exp>; <exp>; <exp>) <block_commands>
// <for_command> -> for(<decl_command>; <exp>; <exp>) <block_commands>
void lexer::for_command(Scope* scope) {
	std::cout << "for command"  << std::endl;
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	Scope* for_scope = new Scope(scope);
	for_scope->parent_scope = scope;
	get_token(t);
	if(tokens.top()->type != BRACKET_OPEN)
		error_handle("(");
	get_token(t);
	int stack_top = tokens.size();
	if(!decl_command(for_scope)) {
		remove_stack_top(stack_top);
		exp(for_scope);
	}
	if(tokens.top()->type != SEMICOLON)
		error_handle(";");
	get_token(t);
	exp(for_scope);
	if(tokens.top()->type != SEMICOLON)
		error_handle(";");
	get_token(t);
	exp(for_scope);
	if(tokens.top()->type != BRACKET_CLOSE)
		error_handle(")");
	get_token(t);
	block_commands(for_scope);
}

// <while_command> -> while(<exp>) <block_commands>
void lexer::while_command(Scope* scope) {
	std::cout << "while command"  << std::endl;
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	Scope* while_scope = new Scope(scope);
	get_token(t);
	if(tokens.top()->type != BRACKET_OPEN)
		error_handle("(");
	get_token(t);
	exp(while_scope);
	get_token(t);
	if(tokens.top()->type != BRACKET_CLOSE)
		error_handle(")");
	get_token(t);
	block_commands(while_scope);
}

// <input_command> -> input(<Lvalue>)
void lexer::input_command(Scope* scope) {
	std::cout << "input command"  << std::endl;
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	get_token(t);
	if(tokens.top()->type != BRACKET_OPEN)
		error_handle("(");
	get_token(t);
	Variable* variable = Lvalue(scope);
	if(tokens.top()->type != BRACKET_CLOSE)
		error_handle(")");
	get_token(t);
}

// <print_command> -> print(<exp>)
void lexer::print_command(Scope* scope) {
	std::cout << "print command"  << std::endl;
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	get_token(t);
	if(tokens.top()->type != BRACKET_OPEN)
		error_handle("(");
	get_token(t);
	exp(scope);
	if(tokens.top()->type != BRACKET_CLOSE)
		error_handle(")");
	get_token(t);
}

// <return_command> -> return <exp> | return
void lexer::return_command(Scope* scope) {
	std::cout << "return command"  << std::endl;
	get_token(t);
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	Var_object* ret;
	if(tokens.top()->type != SEMICOLON) {
		ret = exp(scope);
	}
	if(!convertible(ret, curr_function->return_object))
		error_handle("cannot convert");
}

// <loop_command> -> break | continue
void loop_command(tokenizer* t) {
	get_token(t);
}

// <command> -> <decl_command>; | <if_commmand> | <for_command> | <while_command>
// <command> -> <input_command>; | <print_command>;
// <command> -> <exp>; | <return_command>; | <loop_command>;
void lexer::command(Scope* scope) {
	std::cout << "command"  << std::endl;
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	if(tokens.top()->value == "if")
		if_command(scope);
	else if(tokens.top()->value == "for")
		for_command(scope);
	else if(tokens.top()->value == "while")
		while_command(scope);
	else {
		if(tokens.top()->value == "return")
			return_command(scope);
		else if(tokens.top()->value == "input")
			input_command(scope);
		else if(tokens.top()->value == "print")
			print_command(scope);
		else if(tokens.top()->value == "break" || tokens.top()->value == "continue")
			loop_command(t);
		else if(tokens.top()->type == VAR) {
			int stack_top = tokens.size();
			if(!decl_command(scope)) {
				remove_stack_top(stack_top);
				exp(scope);
			}
		}
		if(tokens.top()->type != SEMICOLON)
			error_handle("command - ;");
		get_token(t);
	}
}

// <commands> -> <command> <commands> | $
void lexer::commands(Scope* scope) {
	std::cout << "commands"  << std::endl;
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	if(tokens.top()->type == CURLY_CLOSE)
		return;
	command(scope);
	commands(scope);
}

// <block_commands> -> {<commands>} | <command>
void lexer::block_commands(Scope* scope) {
	std::cout << "block commands"  << std::endl;
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	if(tokens.top()->type == CURLY_OPEN) {
		get_token(t);
		commands(scope);
		if(tokens.top()->type != CURLY_CLOSE)
			error_handle("}");
		get_token(t);
	} else {
		command(scope);
	}
}

// <func_arguments_list> -> <decl_command>, <func_arguments_list> | <decl_command>
void lexer::func_arguments_list(Scope* scope, Function* function) {
	std::cout << "func arguments list"  << std::endl;
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	if(!decl_command(function->function_scope))
		error_handle("argument");
	function->function_parameters.push_back((Var_object*) function->function_scope->variables.back());
	if(tokens.top()->type == COMMA) {
		get_token(t);
		func_arguments_list(scope, function);
	}
}

// <function_arguments> -> <func_arguments_list> | $
void lexer::function_arguments(Scope* scope, Function* function) {
	std::cout << "function arguments" << std::endl;
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	if(tokens.top()->type == VAR) {
		func_arguments_list(scope, function);
	}
}

// <return_type> -> =data_type | $
void lexer::return_type(Function* function) {
	std::cout << "return type"  << std::endl;
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	if(tokens.top()->value != "=")
		return;
	get_token(t);
	if(tokens.top()->type != DATA_TYPE)
		error_handle("return type");
	function->return_object = new Var_object("", get_type_from_decl(tokens.top()));
	get_token(t);
}

// <function> -> var (<function_arguments>) <return_type> <block_commands>
void lexer::function(Scope* scope) {
	std::cout << "function"  << std::endl;
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	Variable* func = find_var(tokens.top(), scope->variables);
	if(func != nullptr)
		error_handle("already defined");
	Token* var_name = tokens.top();
	Function* function = new Function(tokens.top()->value, new Scope(scope));
	curr_function = function;
	get_token(t);
	if(tokens.top()->type != BRACKET_OPEN)
		error_handle("(");
	get_token(t);
	function_arguments(scope, function);
	if(tokens.top()->type != BRACKET_CLOSE)
		error_handle(")");
	get_token(t);
	return_type(function);
	block_commands(function->function_scope);
	if(scope->parent_scope == nullptr) {
		scope->variables.push_back(function);
		return;
	}
	Variable* find_class = find_var(var_name, scope->parent_scope->variables);
	if(find_class != nullptr && find_class->type == class_) {
		Class *cl = (Class*) find_class;
		if(cl->class_scope == scope && cl->constructor == nullptr) {
			cl->constructor = function;
			function->return_object = new Var_object("", class_instance);
			function->return_object->class_type = cl;
		} else
			error_handle("something wrong, should analyze");
	}
	scope->variables.push_back(function);
	curr_function = nullptr;
}

// <base_list> -> var, <base_list> | var
void lexer::base_list(Scope* scope) {
	std::cout << "base list"  << std::endl;
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	if(tokens.top()->type != VAR)
		error_handle("base class name");
	get_token(t);
	if(tokens.top()->type == COMMA) {
		get_token(t);
		base_list(scope);
	}
}

// <base_classes> -> (<base_list>) | $
void lexer::base_classes(Scope* scope) {
	std::cout << "base classes"  << std::endl;
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	if(tokens.top()->type == CURLY_OPEN)
		return;
	if(tokens.top()->type != BRACKET_OPEN)
		error_handle("(");
	base_list(scope);
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
void lexer::visibility_block(Scope* scope) {
	std::cout << "visibility block"  << std::endl;
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	visibility_specifier(t);
	program_parts(scope);
}

// <class_body> -> <visibility_block> <class_body> | <visibility_block>
void lexer::class_body(Scope* scope) {
	std::cout << "class body"  << std::endl;
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	visibility_block(scope);
	if(tokens.top()->type != CURLY_CLOSE)
		class_body(scope);
}

// <class_decl> -> class var <base_classes> {<class_body>}
void lexer::class_decl(Scope* scope) {
	std::cout << "class decl"  << std::endl;
	get_token(t);
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	if(tokens.top()->type != VAR)
		error_handle("class name");
	Class *c = new Class(tokens.top()->value, new Scope(scope));
	scope->variables.push_back(c);
	get_token(t);
	base_classes(c->class_scope);
	if(tokens.top()->type != CURLY_OPEN)
		error_handle("{");
	get_token(t);
	class_body(c->class_scope);
	if(tokens.top()->type != CURLY_CLOSE)
		error_handle("}");
	get_token(t);
}

// <program_parts> -> <class_decl> <program_parts> | <function> <program_parts>
// <program_parts> -> <decl_command>; <program_parts> | $
void lexer::program_parts(Scope* scope) {
	std::cout << "program parts"  << std::endl;
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	if(tokens.top()->value == "class")
		class_decl(scope);
	else if(tokens.top()->type == VAR) {
		int stack_top = tokens.size();
		if(decl_command(scope)) {
			if(tokens.top()->type != SEMICOLON)
				error_handle(";");
			get_token(t);
		} else {
			remove_stack_top(stack_top);
			function(scope);
		}
	}
	if(tokens.top()->value != "main"
	 && tokens.top()->value != "public"
	 && tokens.top()->value != "private"
	 && tokens.top()->type != CURLY_CLOSE)
		program_parts(scope);
}

// <main_function> -> main (<function_arguments>) <block_commands>
void lexer::main_function(Scope* scope) {
	std::cout << "main function"  << std::endl;
	std::cout << tokens.top()->type << " " << tokens.top()->line << " " << tokens.top()->value << std::endl;
	Function* function = new Function(tokens.top()->value, scope);
	get_token(t);
	if(tokens.top()->type != BRACKET_OPEN)
		error_handle("(");
	get_token(t);
	function_arguments(scope, function);
	if(tokens.top()->type != BRACKET_CLOSE)
		error_handle(")");
	get_token(t);
	block_commands(function->function_scope);
}

// <program> -> <program_parts> <main_function>
// before calling each function, the token that begins that function,
// should be on the top of the tokens stack
// after each function is executed, token that follows that function
// is on the top of the tokens stack
void lexer::program() {
	std::cout << "program"  << std::endl;
	global_scope = new Scope(nullptr);
	get_token(t);
	program_parts(global_scope);
	main_function(global_scope);
	if(tokens.top()->type != END_OF_FILE)
		error_handle("EOF");
}