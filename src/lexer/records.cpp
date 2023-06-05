#include "lexer.hpp"

Variable::Variable() {
	generated_name = "a" + std::to_string(generated++);
}

Function::Function(std::string name_, Scope* scope_) {
	name = name_;
	type = function_;
	function_scope = scope_;
}

Function::Function(std::string name_, Scope* scope, Var_object* return_object_): function_scope(scope) {
	name = name_;
	type = function_;
	return_object = return_object_;
}

Class::Class(std::string name_, Scope* scope) {
	name = name_;
	constructor = nullptr;
	type = class_;
	class_scope = scope;
}

Var_object::Var_object(std::string name_, data_types type_) {
	this->name = name_;
	type = type_;
	class_type = nullptr;
}

Array::Array(std::string name_, std::vector<Var_object*> size_, Var_object* type_) {
	name = name_;
	containing_type = type_;
	type = array_;
	for(size_t i = 0; i < size_.size(); i++) {
		this->size.push_back(size_[i]);
	}
}

Array_element::Array_element(Array_element* arr): Var_object(arr->name, array_element) {
	array_ = arr->array_;
	type = array_element;
	// optimize
	begin_index = arr->begin_index;
	end_index = arr->end_index;
	generated_name = arr->generated_name;
	generated_code = arr->generated_code;
	arr->generated_code = "";
}

Array_element::Array_element(Array* arr): Var_object(arr->name, array_element) {
	array_ = arr;
	type = array_element;
	generated_name = arr->generated_name;
	generated_code = arr->generated_code;
	arr->generated_code = "";
}

bool Array_element::is_one_element() {
	if(begin_index.size() < array_->size.size())
		return false;
	for(size_t i = 0; i < begin_index.size(); i++) {
		if(begin_index[i] != end_index[i])
			return false;
	}
	return true;
}

data_types Var_object::get_type() {
	return type;
}

data_types Array_element::get_type() {
	if(is_one_element())
		return this->array_->containing_type->get_type();
	return type;
}