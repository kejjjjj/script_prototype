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
	Variable() = default;

	//Variable(const Variable&) = delete;
	//Variable(Variable&) = delete;
	Variable(const var_declr_data& init_data);
	void AllocateValues();
	template<typename T> void set_value(const T& _value)
	{
		if (value.buffer.use_count() == NULL) { throw scriptError_t("lvalue: called set_value() without a value.. how?"); }
		*reinterpret_cast<T*>(value.buffer.get()) = _value;
	}
	char get_char() const noexcept { return *reinterpret_cast<char*>	(value.buffer.get()); }
	int get_int() const noexcept { return *reinterpret_cast<int*>	(value.buffer.get()); }
	float get_float() const noexcept { return *reinterpret_cast<float*>	(value.buffer.get()); }

	void print(size_t spaces = 0) const;
	std::string s_getvariabletype() const;

	auto get_type() const noexcept { return type; }
	void set_type(const dataTypes_e _type) noexcept(true) { type = _type; }
	bool is_array() const noexcept { return arrayElements.empty() == FALSE; }
	void resize_array(const size_t newSize);
	void set_array_depth(const size_t newSize);
	void create_array();
	void replace_array(const std::vector<std::unique_ptr<Variable>>&, const size_t length);
	size_t array_depth() const;

	expression_token to_expression();

	std::string identifier;
	VariableValue value;

	std::vector<std::unique_ptr<Variable>> arrayElements;
	size_t numElements = 0;
	
	
	bool isInitialized = false;
	
	Variable(const Variable&) = delete;
	Variable& operator=(const Variable&) = delete;

	//Array arr;
private:
	

	

	dataTypes_e type = dataTypes_e::UNKNOWN;

};

class VariableTable
{
public:

	auto declare_variable(std::unique_ptr<Variable>& v) -> Variable* {
		return table.insert({ v->identifier, std::move(v) }).first->second.get();
	}

	auto find(const std::string& v) {
		auto found = table.find(v);
		return found != table.end() ? std::make_optional(found) : std::nullopt;
	}

	void erase(const std::string& v) noexcept(true) {
		table.erase(v);
	}
	void erase_all() noexcept(true) {
		if (table.empty())
			return;

		std::cout << "\n---- killing local variables: ----\n\n";

		for (auto& i : table)
			std::cout << i.second->identifier << '<' << i.second->s_getvariabletype() << '>' << '\n';

		std::cout << "\n----------------------------------\n\n";

		table.clear();
	}
	void print() const noexcept {
		if (table.empty()) {
			std::cout << "empty script stack\n";
			return;
		}
		std::cout << "---- script stack: ----\n\n";
			
		for (const auto& i : table) {
			i.second->print();
		}

	}
private:
	std::unordered_map<std::string, std::unique_ptr<Variable>> table;
};

#endif