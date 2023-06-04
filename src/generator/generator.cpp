#include "generator.hpp"

bool is_basic(data_types type) {
	return type >= 0 && type <= 12;
}

std::string convert_to_C(data_types type) {
	switch (type)
	{
	case bool_type:
		return "bool";
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
		return "char[]";
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
	file << " " << var->generated_name << " = " << t->value << ";\n";
}

void generate_C::generate_exp(Var_object* var_from, Var_object* var_to, Token* oper){
	file << convert_to_C(var_to->type) << " " << var_to->generated_name << " = " << oper->value << var_from->generated_name << ";\n";
}

void generate_C::generate_exp(Var_object* var_from1, Var_object* var_from2, Var_object* var_to, Token* oper) {
	file << convert_to_C(var_to->type) << " " << var_to->generated_name << " = " << var_from1->generated_name << oper->value << var_from2->generated_name << ";\n";
}

void generate_C::generateH(Var_object* var, Token* t) {
	int numb = t->value.size() - 1;
	file << var->generated_name << " = " << var->generated_name << t->value[0] << numb << ";\n";
}

void generate_C::generateH(Var_object* var, Token* t, Var_object* exp) {
	file << var->generated_name << " = " << var->generated_name << t->value[0] << exp->generated_name << ";\n";
}

// TODO better conversion