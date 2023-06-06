#include "generator.hpp"
#include <iostream>

bool is_basic(data_types type) {
	return type >= 0 && type <= 12;
}

std::string convert_to_C(data_types type) {
	switch (type)
	{
	case bool_type:
		return "char";
	case int8:
		return "char";
	case int16:
		return "short";
	case int32:
		return "int";
	case int64:
		return "long long";
	case uint8:
		return "unsigned char";
	case uint16:
		return "unsigned short";
	case uint32:
		return "unsigned int";
	case uint64:
		return "unsigned long long";
	case float32:
		return "float";
	case float64:
		return "double";
	case float96:
		return "long double";
	case string_type:
		return "char";
	default:
		break;
	}
	std::cerr << "only basic types for now" << std::endl;
	exit(1);
}

std::string qualifier(data_types type) {
	switch (type)
	{
	case bool_type:
		return "hhd";
	case int8:
		return "hhd";
	case int16:
		return "hd";
	case int32:
		return "d";
	case int64:
		return "lld";
	case uint8:
		return "hhu";
	case uint16:
		return "hu";
	case uint32:
		return "u";
	case uint64:
		return "llu";
	case float32:
		return "f";
	case float64:
		return "lf";
	case float96:
		return "L";
	case string_type:
		return "s";
	default:
		break;
	}
	exit(1);
}


void code_generator::set_inside_class(bool inside_class_) {
	inside_class = inside_class_;
}

void code_generator::set_curr_function(Function* curr_function_) {
	curr_function = curr_function_;
}

void code_generator::set_curr_class(Class* curr_class_) {
	curr_class = curr_class_;	
}

void code_generator::write(std::string code) {
	if(inside_class)
		curr_function->generated_code += code;
	else
		file << code;
	file.flush();
}


void generate_C::generate_exp(Var_object* var, Var_object* exp) {
	var->generated_code += exp->generated_code + var->get_name(curr_class, curr_function) + " = " + exp->get_name(curr_class, curr_function) + ";\n";
	exp->generated_code = "";
}

void generate_C::generate_exp(Var_object* var, Token* t) {
	if(is_basic(var->type))
		var->generated_code = convert_to_C(var->type);
	if(var->type == class_instance)
		var->generated_code = "struct " + var->class_type->generated_name;
	if (t->type == STRING)
		var->generated_code += " " + var->generated_name + "[] = \"" + t->value + "\";\n";
	else
		var->generated_code += " " + var->generated_name + " = " + t->value + ";\n";
}

void generate_C::generate_exp(Var_object* var_from, Var_object* var_to, Token* oper){
	var_to->generated_code = var_from->generated_code;
	if(is_basic(var_to->type))
		var_to->generated_code += convert_to_C(var_to->type);
	if(var_to->type == class_instance)
		var_to->generated_code += "struct " + var_to->class_type->generated_name;
	var_to->generated_code += " " + var_to->generated_name;
	if(var_to->type == string_type)
		var_to->generated_code += "[]";
	var_to->generated_code += " = " + oper->value + var_from->get_name(curr_class, curr_function) + ";\n";
	var_from->generated_code = "";
}

void generate_C::generate_exp(Var_object* var_from1, Var_object* var_from2, Var_object* var_to, Token* oper) {
	var_to->generated_code = var_from1->generated_code;
	var_from1->generated_code = "";
	var_to->generated_code += var_from2->generated_code;
	if(is_basic(var_to->type))
		var_to->generated_code += convert_to_C(var_to->type);
	if(var_to->type == class_instance)
		var_to->generated_code += "struct " + var_to->class_type->generated_name;
	var_to->generated_code += " " + var_to->generated_name;
	if(var_to->type == string_type)
		var_to->generated_code += "[]";
	var_to->generated_code += " = " + var_from1->get_name(curr_class, curr_function) + oper->value + var_from2->get_name(curr_class, curr_function) + ";\n";
	var_from2->generated_code = "";
}

void generate_C::generateH(Var_object* var, Token* t) {
	int numb = t->value.size() - 1;
	var->generated_code += var->get_name(curr_class, curr_function) + " = " + var->get_name(curr_class, curr_function) + t->value[0] + std::to_string(numb) + ";\n";
}

void generate_C::generateH(Var_object* var, Token* t, Var_object* exp) {
	var->generated_code += exp->generated_code;
	var->generated_code += var->get_name(curr_class, curr_function) + " = " + var->get_name(curr_class, curr_function) + t->value[0] + exp->get_name(curr_class, curr_function) + ";\n";
	exp->generated_code = "";
}

void generate_C::generate_var(Var_object* var) {
	if(is_basic(var->type))
		var->generated_code = convert_to_C(var->type);
	if(var->type == class_instance)
		var->generated_code = "struct " + var->class_type->generated_name;
	if(var->type == string_type)
		var->generated_code += "[]";
	var->generated_code += " " + var->generated_name + ";\n";
}

void generate_C::generate_var(Array* var) {
	var->generated_code = "";
	for(auto s: var->size) {
		var->generated_code += s->generated_code;
		s->generated_code = "";
	}
	var->generated_code += convert_to_C(var->containing_type->type) + " " + var->generated_name;
	for(auto s: var->size)
		var->generated_code += "[" + s->get_name(curr_class, curr_function) + "]";
	var->generated_code += ";\n";
}

void generate_C::generate_undefined_exp(Var_object* var, Var_object* exp) {
	if(is_basic(var->type))
		var->generated_code = convert_to_C(var->type);
	if(var->type == class_instance)
		var->generated_code = "struct " + var->class_type->generated_name;
	var->generated_code += " " + var->generated_name;
	
	if(var->type == string_type)
		var->generated_code += "[]";
	var->generated_code += ";\n";
	generate_exp(var, exp);
}

void generate_C::write_exp(std::string code) {
	write(code);
}

void generate_C::generate_if(Var_object* exp) {
	write("if(" + exp->get_name(curr_class, curr_function) + ") {\n");
}

void generate_C::generate_if_not(Var_object* exp) {
	write("if(!" + exp->get_name(curr_class, curr_function) + ") {\n");
}

void generate_C::end_block() {
	write("}\n");
}

void generate_C::generate_else() {
	write("else {\n");
}

void generate_C::input(Var_object* var) {
	write("scanf(\"%" + qualifier(var->get_type()) + "\", &" + var->get_name(curr_class, curr_function) + ");\n");
}

void generate_C::print(Var_object* var) {
	write(var->generated_code);
	var->generated_code = "";
	write("printf(\"%" + qualifier(var->get_type()) + "\", " + var->get_name(curr_class, curr_function) + ");\n");
}

void generate_C::for_begin() {
	write("goto for" + std::to_string(for_numbs) + "_cond;\n");
	write("for" + std::to_string(for_numbs) + "_begin: ;\n");
	curr_fors.push(for_numbs++);
}

void generate_C::for_cond(Var_object* exp, std::string code) {
	write("for" + std::to_string(curr_fors.top()) + "_cond: ;\n");
	write(code);
	generate_if(exp);
	write("\tgoto for" + std::to_string(curr_fors.top()) + "_begin;\n");
	end_block();
	write("for" + std::to_string(curr_fors.top()) + "_end: ;\n");
	curr_fors.pop();
}

void generate_C::for_continue() {
	write("for" + std::to_string(curr_fors.top()) + "_cont: ;\n");
}

void generate_C::break_loop() {
	if(curr_fors.size() == 0) {
		std::cerr << "a continue statement may only be used within a loop" << std::endl;
		exit(1);
	}
	write("goto for" + std::to_string(curr_fors.top()) + "_end;\n");
}

void generate_C::continue_loop() {
	if(curr_fors.size() == 0) {
		std::cerr << "a continue statement may only be used within a loop" << std::endl;
		exit(1);
	}
	write("goto for" + std::to_string(curr_fors.top()) + "_cont;\n");
}


void generate_C::return_() {
	write("return;\n");
}

void generate_C::return_(Var_object* ret) {
	write(ret->generated_code);
	write("return " + ret->get_name(curr_class, curr_function) + ";\n");
}

void generate_C::function_decl(Function* f) {
	if(f->return_object == nullptr)
		write("void");
	else if(is_basic(f->return_object->type))
		write(convert_to_C(f->return_object->type));
	else
		write("struct " + f->return_object->class_type->generated_name);
	write(" " + f->generated_name + " (");
	if(inside_class && curr_class->constructor != f)
		write("struct " + curr_class->generated_name + "* b" + curr_class->generated_name);
	for(size_t i = 0; i < f->function_parameters.size(); i++) {
		if(i != 0 || (inside_class && curr_class->constructor != f))
			write(", ");
		if(is_basic(f->function_parameters[i]->type))
			write(convert_to_C(f->function_parameters[i]->type));
		if(f->function_parameters[i]->type == class_instance)
			write("struct " + f->function_parameters[i]->class_type->generated_name + "*");
		write(" " + f->function_parameters[i]->generated_name);
		f->function_parameters[i]->generated_code = "";
	}
	write(") {\n");
	if (inside_class && curr_class->constructor == f)
		write("struct " + curr_class->generated_name + " b" + curr_class->generated_name + ";\n");
}

void generate_C::constructor_return() {
	write("return b" + curr_class->generated_name + ";\n");
}

void generate_C::main_decl(Function* f) {
	if(f->return_object == nullptr)
		file << "void";
	else
		file << convert_to_C(f->return_object->type);
	file << " main (";
	if(f->function_parameters.size() != 0)
		file << "int argc, char** argv";
	file << ") {\n";
	if(f->function_parameters.size() != 0) {
		file << "if(argc <" << f->function_parameters.size() + 1 << ") {\n";
		file << "\tfprintf(stderr, \"error - too few arguments\");\n";
		file << "\texit(1);\n}\n";
	}
	for(size_t i = 0; i < f->function_parameters.size(); i++) {
		file << f->function_parameters[i]->generated_code;
		f->function_parameters[i]->generated_code = "";
		file << f->function_parameters[i]->generated_name << " =  argv[" << i + 1 << "];\n";
	}
}

void generate_C::generate_class(Class* c) {
	write("struct " + c->generated_name + " {\n");
	// kako dobre petlje, skoro pa bi imalo smisla napraviti dvije liste hehe
	for(auto var: c->class_scope->variables) {
		if(var->type != function_) {
			generate_var((Var_object*) var);
			write(var->generated_code);
			var->generated_code = "";
		}
	}
	write("};\n");
	for(auto var: c->class_scope->variables) {
		if(var->type == function_) {
			write(var->generated_code);		
		}
	}
}

void generate_C::function_call(Function* f, std::vector<Var_object*> args) {
	f->return_object->generated_name = "a" + f->return_object->generated_name;
	generate_var(f->return_object);
	for(auto arg: args)
		f->return_object->generated_code += arg->generated_code;
	f->return_object->generated_code += f->return_object->generated_name + " = " + f->generated_name + "(";
	for(int i = args.size()-1; i >= 0; i--) {
		if(args[i]->type == class_instance)
			f->return_object->generated_code += "&";
		f->return_object->generated_code += args[i]->get_name(curr_class, curr_function);
		if(i != 0)
			f->return_object->generated_code += ", ";
	}
	f->return_object->generated_code += ");\n";
}