#include "generator.hpp"

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

void generate_C::generate_exp(Var_object* var, Var_object* exp) {
	file << var->generated_name << " = " << exp->generated_name << ";\n";
}

void generate_C::generate_exp(Var_object* var, Token* t) {
	if(is_basic(var->type))
		file << convert_to_C(var->type);
	else 
		//TODO
		return;
	if (t->type == STRING)
		file << " " << var->generated_name << "[] = \"" << t->value << "\";\n";
	else
		file << " " << var->generated_name << " = " << t->value << ";\n";
}

void generate_C::generate_exp(Var_object* var_from, Var_object* var_to, Token* oper){
	file << convert_to_C(var_to->type) << " " << var_to->generated_name;
	if(var_to->type == string_type)
		file << "[]";
	file << " = " << oper->value << var_from->generated_name << ";\n";
}

void generate_C::generate_exp(Var_object* var_from1, Var_object* var_from2, Var_object* var_to, Token* oper) {
	file << convert_to_C(var_to->type) << " " << var_to->generated_name;
	if(var_to->type == string_type)
		file << "[]";
	file << " = " << var_from1->generated_name << oper->value << var_from2->generated_name << ";\n";
}

void generate_C::generateH(Var_object* var, Token* t) {
	int numb = t->value.size() - 1;
	file << var->generated_name << " = " << var->generated_name << t->value[0] << numb << ";\n";
}

void generate_C::generateH(Var_object* var, Token* t, Var_object* exp) {
	file << var->generated_name << " = " << var->generated_name << t->value[0] << exp->generated_name << ";\n";
}

void generate_C::generate_var(Var_object* var) {
	file << convert_to_C(var->type);
	if(var->type == string_type)
		file << "[]";
	file << " " << var->generated_name << ";\n";
}

void generate_C::generate_var(Array* var) {
	file << convert_to_C(var->containing_type->type) << " " << var->generated_name;
	for(auto s: var->size) {
		file << "[" << s->generated_name << "]";
	}
	file << ";\n";
}

void generate_C::generate_undefined_exp(Var_object* var, Var_object* exp) {
	file << convert_to_C(var->type) << " ";
	generate_exp(var, exp);
}

void generate_C::generate_if(Var_object* exp) {
	file << "if(" << exp->generated_name << ") {\n";
}

void generate_C::generate_if_not(Var_object* exp) {
	file << "if(!" << exp->generated_name << ") {\n";
}

void generate_C::end_block() {
	file << "}\n";
}

void generate_C::generate_else() {
	file << "else {\n";
}

void generate_C::while_begin() {
	file << "while_" << while_numbs << ": ;\n";
	curr_whiles.push(while_numbs++);
}

void generate_C::while_branch(Var_object* exp) {
	generate_if_not(exp);
	file << "goto while_end" << curr_whiles.top() << ";\n";
	end_block();
}

void generate_C::while_end() {
	file << "goto while_" << curr_whiles.top() << ";\n";
	file << "while_end" << curr_whiles.top() << ": ;\n";
	curr_whiles.pop();
}

void generate_C::input(Var_object* var) {
	file << "scanf(\"%" << qualifier(var->get_type()) << "\", &" << var->generated_name << ");\n";
}

void generate_C::print(Var_object* var) {
	file << "printf(\"%" << qualifier(var->get_type()) << "\", " << var->generated_name << ");\n";
}