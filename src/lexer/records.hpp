#include "../tokenizer/tokenizer.hpp"
#include <string>
#include <vector>

enum data_types {
	undefined = -1,
	bool_type,
	int8,
	int16,
	int32,
	int64,
	uint8,
	uint16,
	uint32,
	uint64,
	float32,
	float64,
	float96,
	string_type,
	class_,
	function_,
	class_instance,
	array_,
	array_element
};

class Scope;
class Var_object;
class Variable {
	public:
		std::string name;
		data_types type;
		virtual Variable* var_extend(tokenizer* t, Scope* scope) = 0;
};
class Scope {
	public:
		std::vector <Variable*> variables;
		Scope* parent_scope;
		Scope(Scope* parent_scope_): parent_scope(parent_scope_) {}
};
class Function: public Variable {
	public:
		Var_object* return_object;
		Scope* function_scope;
		std::vector <Var_object* > function_parameters;
		Function(std::string name_, Scope* scope, Var_object* return_object_): function_scope(scope) {
			name = name_;
			type = function_;
			return_object = return_object_;
		}
		Function(std::string name_, Scope* scope): function_scope(scope) {
			name = name_;
			type = function_;
		}
		virtual Variable* var_extend(tokenizer* t, Scope* scope);
};
class Class: public Variable {
	public:
		Scope* class_scope;
		Function* constructor;
		std::unordered_set<Class*> base_classes;
		Class(std::string name_, Scope* scope) {
			name = name_;
			constructor = nullptr;
			type = class_;
			class_scope = scope;
		}
		virtual Variable* var_extend(tokenizer* t, Scope* scope);
};
class Var_object: public Variable {
	public:
		Class* class_type;
		Var_object(std::string name_, data_types type_) {
			this->name = name_;
			type = type_;
			class_type = nullptr;
		}
		virtual Variable* var_extend(tokenizer* t, Scope* scope);
};
class Array: public Variable {
	public:
		std::vector<Var_object*> size;
		Var_object* containing_type;
		Array(std::string name_, std::vector<Var_object*> size_, Var_object* type) {
			name = name_;
			containing_type = type;
			for(size_t i = 0; i < size_.size(); i++) {
				this->size.push_back(size_[i]);
			}
		}
		virtual Variable* var_extend(tokenizer* t, Scope* scope);
};

class Array_element: public Var_object {
	private:
		std::vector<Var_object*> begin_index;
		std::vector<Var_object*> end_index;
	public:
		void add_begin(Var_object* begin);
		void add_end(Var_object* end);
		Array* array_;
		// TODO check if begin and end index valid
		Array_element(Array_element* arr): Var_object(arr->name, array_element) {
			array_ = arr->array_;
			// optimize
			begin_index = arr->begin_index;
			end_index = arr->end_index;
		}
		Array_element(Array* arr): Var_object(arr->name, array_element) {
			array_ = arr;
		}
		virtual Variable* var_extend(tokenizer* t, Scope* scope);
		bool is_one_element() {
			if(begin_index.size() < array_->size.size())
				return false;
			for(int i = 0; i < begin_index.size(); i++) {
				if(begin_index[i] != end_index[i])
					return false;
			}
			return true;
		}
};