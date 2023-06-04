#include "lexer.hpp"

Variable::Variable() {
	generated_name = "a{0}" + std::to_string(generated++);
}

Function::Function(std::string name_, Scope* scope): function_scope(scope) {
	name = name_;
	type = function_;
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

Array::Array(std::string name_, std::vector<Var_object*> size_, Var_object* type) {
	name = name_;
	containing_type = type;
	for(size_t i = 0; i < size_.size(); i++) {
		this->size.push_back(size_[i]);
	}
}

Array_element::Array_element(Array_element* arr): Var_object(arr->name, array_element) {
	array_ = arr->array_;
	// optimize
	begin_index = arr->begin_index;
	end_index = arr->end_index;
}

Array_element::Array_element(Array* arr): Var_object(arr->name, array_element) {
	array_ = arr;
}

bool Array_element::is_one_element() {
	if(begin_index.size() < array_->size.size())
		return false;
	for(int i = 0; i < begin_index.size(); i++) {
		if(begin_index[i] != end_index[i])
			return false;
	}
	return true;
}