#pragma once

#ifndef __variabl_
#define __variabl_

#include "pch.hpp"
#include "rvalue.hpp"

struct var_declr_data
{
	dataTypes_e type = dataTypes_e::UNKNOWN;
	const std::string& name;
	size_t size = 0ull; //optional unless the datatype is not a constant size

};
struct expression_token;

class Variable
{
public:
	Variable() = delete;
	//Variable(const Variable&) = delete;
	//Variable(Variable&) = delete;
	Variable(const var_declr_data& init_data);
	
	template<typename T> void set_value(const T& _value)
	{
		if (value.buffer.use_count() == NULL) { throw scriptError_t("lvalue: called set_value() without a value.. how?"); }
		*reinterpret_cast<T*>(value.buffer.get()) = _value;
	}
	char get_char() const noexcept { return *reinterpret_cast<char*>	(value.buffer.get()); }
	int get_int() const noexcept { return *reinterpret_cast<int*>	(value.buffer.get()); }
	float get_float() const noexcept { return *reinterpret_cast<float*>	(value.buffer.get()); }

	void print(size_t spaces = 0) const;
	auto get_type() const noexcept { return type; }

	expression_token to_expression();

	std::string identifier;
	VariableValue value;
private:
	
	dataTypes_e type = dataTypes_e::UNKNOWN;

};

class VariableTable
{
public:
	static VariableTable& getInstance() {
		static VariableTable instance;
		return instance;
	}
	auto declare_variable(const Variable& v) -> Variable* {
		return &table.insert(std::make_pair(v.identifier, v)).first->second;
	}

	auto find(const std::string& v) {
		auto found = table.find(v);
		return found != table.end() ? std::make_optional(found) : std::nullopt;
	}

	void erase(const std::string& v) noexcept(true) {
		table.erase(v);
	}

	void print() const noexcept {
		if (table.empty()) {
			std::cout << "empty script stack\n";
			return;
		}
		std::cout << "script stack:\n\n";
			
		for (const auto& i : table) {
			i.second.print();
		}

	}
private:
	std::unordered_map<std::string, Variable> table;
};

#endif